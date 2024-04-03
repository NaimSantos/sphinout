#include "JSphVisco.h"
#include "Functions.h"
#include "JReadDatafile.h"
#include <cstring>
#include <float.h>

using namespace std;

/// Constructor:
JSphVisco::JSphVisco(){
	ClassName="JSphVisco";
	Times=NULL;
	Values=NULL;
	Reset();
}

// Destructor:
JSphVisco::~JSphVisco(){
	Reset();
}

// Initialisation of variables:
void JSphVisco::Reset(){
	delete[] Times;
	Times=NULL;
	delete[] Values;
	Values=NULL;
	File="";
	Size=Count=Position=0;
}

// Resizes allocated space for values:
void JSphVisco::Resize(unsigned size){
	Times=fun::ResizeAlloc(Times,Count,size);
	Values=fun::ResizeAlloc(Values,Count,size);
	Size=size;
}

// Returns the allocated memory:
unsigned JSphVisco::GetAllocMemory()const{
	unsigned s=0;
	if(Times)
		s+=sizeof(float)*Size;
	if(Values)
		s+=sizeof(float)*Size;
	return(s);
}

// Loads viscosity values for different instants (in secods):
void JSphVisco::LoadFile(std::string file){
	const char met[]="LoadFile";
	Reset();
	JReadDatafile rdat;
	rdat.LoadFile(file,FILESIZEMAX);
	const unsigned rows=rdat.Lines()-rdat.RemLines();
	Resize(rows);
	for(unsigned r=0;r<rows;r++){
		Times[r]=rdat.ReadNextFloat(false);
		Values[r]=rdat.ReadNextFloat(true);
		//printf("FileData[%u]>  t:%f  ang:%f\n",r,Times[r],Values[r]);
	}
	Count=rows;
	if(Count<2)
		RunException(met,"Cannot be less than two values.",file);
	File=file;
}

// Returns the viscosity value for the indicated instant:
float JSphVisco::GetVisco(float timestep){
	float ret=0.f;
	//-Searches indicated interval of time.
	float tini=Times[Position];
	float tnext=(Position+1<Count? Times[Position+1]: tini);
	for(;tnext<timestep&&Position+2<Count;Position++){
		tini=tnext;
		tnext=Times[Position+2];
	}
	//-Computes dt for the indicated instant.
	if(timestep<=tini)
		ret=Values[Position];
	else if(timestep>=tnext)
		ret=Values[Position+1];
	else{
		const double tfactor=double(timestep-tini)/double(tnext-tini);
		float vini=Values[Position];
		float vnext=Values[Position+1];
		ret=float(tfactor*(vnext-vini)+vini);
	}
	return(ret);
}




