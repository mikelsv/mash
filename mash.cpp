#define PROJECTNAME "mash"
#define PROJECTVER PROJECTNAME ## _versions
#define USEMSV_GENERALCPP

#define USEMSV_HASH
#define USEMSV_MSL_FL
#define USEMSV_PCRE
#define USEMSV_CONSOLE

class msl_fl_extfunc;
msl_fl_extfunc *msl_fl_extfunc_f = 0;

#define MSL_FL_EXTFUNC_DEFAULT msl_fl_extfunc_f

#include "msvcore2/msvcore.cpp"

Versions PROJECTVER[]={
	// new version to up
	"0.0.0.1", "08.12.2016 14:39"
};

#include "mash.h"

class MashPoint{
public:
	MString point;
};

int main(int args, char* arg[]){
	msvcoremain(args, arg);
	print(PROJECTNAME, " v.", PROJECTVER[0].ver, " (", PROJECTVER[0].date, ").\r\n");

	mash_msl_fl_extfunc mash_extfunc;
	msl_fl_extfunc_f = &mash_extfunc;

	OList<MashPoint> points;
	msl_fl msl;
	int ret = 0;

	msl.OutToCon();


	//// match
	//pcre2_match_data *match_data = pcre2_match_data_create(100, NULL);
	//size_t *ovector = pcre2_get_ovector_pointer(match_data);
	//int perrcode; size_t erroffset;

	//VString rule = ".*.sdf";
	//VString line = "ololo.sdf";

	//pcre2_code *p = pcre2_compile(rule, rule, 0, &perrcode, &erroffset, 0);

	//int f = pcre2_match(p, line, line, 0, 0, match_data, 0);

	//unsigned char buf[S1K];
	//pcre2_get_error_message(perrcode, buf, S1K);
	//// End test
	
	// Get args
	ILink &link = msvcorestate.link;

	// Get first option
	VString path = link.GetArg();

	if(!path){
		print("Please set file name.\r\n");
		return -1;
	}

	ILink mlink(path);
	chdir(SString(mlink.GetProtoDomainPath()));

	// Args
	msl_value *val = msl.SetValue("argv", "");

	for(int i = 0; i < args; i ++)
		val->Set(itos(i), arg[i]);

	// Run
	msl.DoFile(mlink.file);
	print(msl.GetOutput());

	// Get points
	while(VString point = link.GetArg()){
		MashPoint * poi = points.NewE();
		poi->point = point;
	}

	MashPoint * poi = 0;
	while(poi = points.Next(poi)){
		//DoMash();
	}

	if(!ret){
		//print("Error. Exit.\r\n");
	}

	return !ret;
}