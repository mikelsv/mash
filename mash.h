MString MashFilesIgnore(VString files, VString ignore);
MString MashFilesSelect(VString files, VString reg);
MString MashFilesDel(VString files, VString ignore);
int MashMkDir(VString dir, VString path);
void HttpRequest(msl_value &val, msl_value &rval);

class mash_msl_fl_extfunc : public msl_fl_extfunc{
public:
	virtual int DoCodeFunctionExec(VString name, msl_fl_fargs &args, msl_value &val){
		// exec
		//if(name=="print" || name=="echo"){
		//	//for(int i=0; i<args.Sz(); i++){
		//		//SetOutput(args[i].val.val);
		//		//print(args[i].val.val);
		//	//}
		//	//return 1;
		//}

		if(name == "mash_files_ignore" && args.Sz() == 2){
			val.val = MashFilesIgnore(args[0].val.val, args[1].val.val);
			return 1;
		}

		if(name == "mash_files_select" && args.Sz() == 2){
			val.val = MashFilesSelect(args[0].val.val, args[1].val.val);
			return 1;
		}

		if(name == "mash_files_del" && args.Sz() == 2){
			val.val = MashFilesDel(args[0].val.val, args[1].val.val);
			return 1;
		}

		if(name == "mash_mkdir" && args.Sz() == 2){
			val.val = itos(MashMkDir(args[0].val.val, args[1].val.val));
			return 1;
		}

		if(name == "mash_buildserver" && args.Sz() == 1){
			MashTestServer(args[0].val.val);
			return 1;
		}

		if(name == "mash_httprequest" && args.Sz() == 1){
			HttpRequest(args[0].val, val);
			return 1;
		}

		if(name == "testextfunc"){
			val.val="ok";
			return 1;
		}

		return 0;
	}
};


MString MashFilesIgnore(VString files, VString ignore){
	IList <VString> list;

	SString tmp;
	ignore = tmp = Replace(ignore, "*", ".*");	
	
	while(files){
		VString file = PartLine(files, files,  "\r\n");

		list.n() = file;
		list.Added();
	}

	// match
	pcre2_match_data *match_data = pcre2_match_data_create(100, NULL);
	size_t *ovector = pcre2_get_ovector_pointer(match_data);
	int perrcode; size_t erroffset;

	while(ignore){
		VString rule = PartLine(ignore, ignore,  "\r\n");

		if(!rule || rule[0] == '#')
			continue;

		pcre2_code *p = pcre2_compile(rule, rule, 0, &perrcode, &erroffset, 0);

		for(unsigned int i = 0; i < list.Size(); i ++){
			int f = pcre2_match(p, list[i], list[i], 0, 0, match_data, 0);

			if(f > 0){
				//print("Del: ", list[i], ". ", rule, "\r\n");
				list.Del(i);
				i --;
			}

			if(perrcode != 100 && f < 0)
				print("Pcre2 error!\r\n");
		}

		pcre2_code_free(p);
	}

	pcre2_match_data_free(match_data);

	LString ls;
	for(unsigned int i = 0; i < list.Size(); i ++){
		if(i)
			ls + "\r\n";
		ls + list[i];
	}

	return (VString)ls;
}

class VStringInt{
public:
	VString key;
	int val;
};


MString MashFilesSelect(VString files, VString reg){
	IList<VStringInt> list;

	SString tmp;
	reg = tmp = Replace(reg, "*", ".*");	
	
	while(files){
		VString file = PartLine(files, files,  "\r\n");

		list.n().key = file;
		list.n().val = 0;
		list.Added();
	}

	// match
	pcre2_match_data *match_data = pcre2_match_data_create(100, NULL);
	size_t *ovector = pcre2_get_ovector_pointer(match_data);
	int perrcode; size_t erroffset;

	while(reg){
		VString rule = PartLine(reg, reg, "\r\n");

		if(!rule || rule[0] == '#')
			continue;

		pcre2_code *p = pcre2_compile(rule, rule, 0, &perrcode, &erroffset, 0);

		for(unsigned int i = 0; i < list.Size(); i ++){
			int f = pcre2_match(p, list[i].key, list[i].key, 0, 0, match_data, 0);

			if(f > 0){
				list[i].val = 1;
			}

			if(perrcode != 100 && f < 0)
				print("Pcre2 error!\r\n");
		}

		pcre2_code_free(p);
	}

	pcre2_match_data_free(match_data);

	LString ls;
	for(unsigned int i = 0; i < list.Size(); i ++){
		if(!list[i].val)
			continue;

		if(ls)
			ls + "\r\n";
		ls + list[i].key;
	}

	return (VString)ls;
}


MString MashFilesDel(VString files, VString ignore){
	IList <VString> list;
	
	while(files){
		VString file = PartLine(files, files,  "\r\n");

		list.n() = file;
		list.Added();
	}

	while(ignore){
		VString rule = PartLine(ignore, ignore,  "\r\n");

		for(unsigned int i = 0; i < list.Size(); i ++){
			if(list[i] == rule){
				list.Del(i);
				i --;
			}
		}
	}

	LString ls;
	for(unsigned int i = 0; i < list.Size(); i ++){
		if(i)
			ls + "\r\n";
		ls + list[i];
	}

	return (VString)ls;
}


int MashMkDir(VString dir, VString path){
	unsigned char *ln = path.data + dir.size() + 1, *to = path.endu(), *p = to;

	//return MkDir(VString(path.data, p - path.data));

	while(p >= ln){
		if(p == to || *p =='/' || *p == '\\')
			if(MkDir(VString(path.data, p - path.data)))
				break;
		p --;
	}

	p ++;

	while(p <= to){
		if(p == to || *p =='/' || *p == '\\')
			if(!MkDir(VString(path.data, p - path.data)))
				return 0;
		p ++;
	}

	return 1;
}


void HttpRequest(msl_value &val, msl_value &rval){
	GetHttp gp;
	TString fdata;

	// request - request.
	// sendfile - file for send.
	// return recv data.

	if(!val["request"]){
		rval.Set("error", "request is empty");
		rval.Set("status", "0");
		return ;
	}

	if(val["sendfile"]){
		gp.SetPost(fdata = LoadFile(val["sendfile"]));
	}

	MString req = val["request"];
	if(req.str(7) != "http://" && req.str(8) != "https://"){
		req.Add("http://", req);
	}

	gp.SetTimeout(300);

	int r = gp.Request(req);
	if(!r){
		rval.Set("error", "request failed");
		rval.Set("status", "0");
		return ;
	}

	rval.Set("status", "1");
	rval.Set("result", gp.GetData());

	return ;
}