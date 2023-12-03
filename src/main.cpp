#define STDCPP_IMPLEMENTATION
#define USE_WINAPI
#include <stdcpp.hpp>

static bool force = false;
static std::string program;
static std::string proj_name;
static bool wants_help = false;
static bool quiet = false;
static bool should_list = false;
static bool dir = false;
static bool wants_thing = false;
static bool wants_search = false;
static std::string search_string;
static std::string things[] = {
  "uwu\n",

  " ∩ ∩ \n"
  "(uwu)\n",

  "(._.)\n",

  ":D\n",

  ":<\n",

  ":>\n",

  ">:(\n",

  ":O\n",

  "(-_-)\n",

  "(^.^)\n",

  "(O.O)\n",

  ":3\n",

  ";3\n",

  ":P\n",

  ">.<\n",
};
static constexpr size_t things_size = (sizeof(things) / sizeof(things[0]));
// TODO: Have some sort of marker in the project folder to identify
// if it is an valid project. Maybe use the .topdir file?

void usage(std::ostream& os, const std::string& program){
  fprint(os, "Usage: {} [options] [subcommand]\n", program);
}

void help(const std::string& program){
  usage(std::cout, program);
  print("\n"
	"  Options:\n"
	"    /?,/h                         prints this help.\n"
	"    /q                            be quiet.\n"
	"    /F                            force the default value on all confirmations.\n"
	"    /p <proj-name>                specifies the name of the project.\n"
	"Note that you can use `-` as a prefix for flags as well.\n"
	"\n"
	"  Subcommands:\n"
	"    list                          lists all of the projects that is in the cpp_dir. (basically all folders in there *for now*)\n"
	"    help                          prints this help.\n"
	"    dir                           runs emacs on the cpp_dir.\n"
	"    search <wildcard-string>      searches the cpp_dir and lists the projects that match the provided wildcard-string.\n"
	"    ???                  ???\n"
	"\n"
	"Program to make a cpp project.\n"
	"Made by Momoyon.\n");
}

bool confirmation(const std::string question, bool _default=true){
  if (force) return _default;
  std::string response = "AAA";
  response = str::tolower(response);
  auto valid = [&](const std::string& str){ return str=="" || str=="y" || str=="yes" || str=="n" || str=="no"; };

  print("{} [yes/no]{{default: {}}}", question, (_default  ? "yes" : "no"));
  std::getline(std::cin, response);
  response = str::tolower(response);

  while (!valid(response)){
    print("Please enter yes or no\n");
    print("{} [yes/no]{{default: {}}}", question, (_default  ? "yes" : "no"));
    std::getline(std::cin, response);
    response = str::tolower(response);
  }
  ASSERT(valid(response));
  return ((_default && response=="") || response=="y" || response=="yes") ? true : false;
}

std::vector<std::string> search(std::string needle, const std::vector<std::string>& haystack, bool icase = false){
  std::vector<std::string> found;
  if (icase) needle = str::tolower(needle);
  if (needle.find('*') == std::string::npos){
    for (auto& h : haystack){
      std::string hay = (icase ? str::tolower(h) : h);
      if (needle == hay){
	found.push_back(h);
      }
    }
  } else {
    if (needle.starts_with("*") && needle.ends_with("*")){
      needle = str::lremove(needle);
      needle = str::rremove(needle);
      for (auto& h : haystack){
	std::string hay = (icase ? str::tolower(h) : h);
	if (hay.find(needle) != std::string::npos){
	  found.push_back(h);
	}
      }
    } else if (needle.starts_with("*")){
      needle = str::lremove(needle);
      for (auto& h : haystack){
	std::string hay = (icase ? str::tolower(h) : h);
	if (hay.ends_with(needle)){
	  found.push_back(h);
	}
      }
    } else if (needle.ends_with("*")){
      needle = str::rremove(needle);
      for (auto& h : haystack){
	std::string hay = (icase ? str::tolower(h) : h);
	if (hay.starts_with(needle)){
	  found.push_back(h);
	}
      }
    } else {
      // * is somewhere in the middle
    }
  }
  return found;
}

int test_search(){
  std::vector<std::string> haystack = win::get_dirs_in_dir("D:\\");
  std::string needle = "b*";
  auto found = search(needle, haystack, true);
  if (found.empty()){
    print("INFO: Couldn't find `{}` in haystack!\n", needle);
    return 1;
  }
  print("Found:\n");
  for (auto f : found){
    print("  `{}`\n", f);
  }

  return 0;
}


int main(int argc, char* argv[]){
  // return test_search();

  srand(unsigned int(time(0)));
  ARG();
  program = arg.pop();

  while (arg){
    std::string a = arg.pop();
    if (a[0] == '/' || a[0] == '-'){
      std::string prefix = str::lpop(a);
      a = str::lremove(a);

      if (a == "?" || a == "h"){
	wants_help = true;
      } else if (a == "q"){
	quiet = true;
      } else if (a == "F"){
	force = true;
      } else if (a == "p"){
        if (!arg){
	  fprint(std::cerr, "ERROR: Project name not provdied after flag `{}{}`\n", prefix, a);
	  exit(1);
	}
	proj_name = arg.pop();
      } else {
	fprint(std::cerr, "ERROR: Unknown flag `{}{}`\n", prefix, a);
	exit(1);
      }
    } else {
      if (a == "list"){
	should_list = true;
      } else if (a == "help"){
	wants_help = true;
      } else if (a == "dir"){
	dir = true;
      } else if (a == "???"){
	wants_thing = true;
      } else if (a == "search"){
	wants_search = true;
	if (!arg){
	  fprint(std::cerr, "ERROR: Search string not provided for subcommand `{}`\n", a);
	  exit(1);
	}
	search_string = arg.pop();
      } else {
	fprint(std::cerr, "ERROR: Unknown subcommand `{}`\n", a);
	exit(1);
      }
    }
  }

  std::string cpp_dir = get_env("CppDir");
  if (cpp_dir.empty()){
    print("WARNING: `CppDir` environment variable is not defined making the project in the current directory...\n");
  }
  if (cpp_dir[cpp_dir.size()-1] != '\\') cpp_dir += '\\';

  const std::string projs_dir = (cpp_dir.empty() ? win::get_current_dir() : cpp_dir);
  std::vector<std::string> projs = win::get_dirs_in_dir(projs_dir);

  if (wants_help){
    help(program);
    exit(0);
  }

  if (wants_thing){
    const size_t n = rand() % things_size;
    ASSERT(n < things_size);

    print("{}", things[n]);

    exit(0);
  }

  if (wants_search){
    if (!quiet) print("INFO: Searching for string `{}` in `{}`...\n", search_string, projs_dir);
    auto found = search(search_string, projs, true);
    if (found.empty()){
      fprint(std::cerr, "ERROR: Could not find any projects named `{}` in `{}`...\n", search_string, projs_dir);
      exit(1);
    }
    print("Found:\n");
    for (auto& f : found){
      print("  `{}`\n", f);
    }
    exit(0);
  }

  if (should_list){
    print("Projects in {}:\n", projs_dir);
    for (auto& d : projs){
      if (d.size() > 0 && d[0] == '.') continue;
      print("  {}\n", d);
    }
    print("\n"
	  "Total {} projects\n", projs.size());
    exit(0);
  }

  if (dir){
    if (!quiet) print("INFO: running emacs on `{}`\n", projs_dir);
    win::run_async("runemacs", projs_dir);
    exit(0);
  }

  if (proj_name.empty()){
    help(program);
    exit(0);
  }

  ASSERT(cpp_dir[cpp_dir.size()-1] == '\\');
  std::string full_proj_path = FMT("{}{}", projs_dir, proj_name);

  // check if project already exists
  if (fs::exists(fs::path(full_proj_path))){
    if (confirmation(FMT("INFO: Project with name `{}` already exists. Overwrite it?", proj_name), false)){
      // delete the existing folder
      if (!quiet) print("INFO: Deleting dir `{}`...\n", full_proj_path);
      if (!fs::remove_all(fs::path(full_proj_path))){
	fprint(std::cerr, "ERROR: Could not delete `{}`...\n", full_proj_path);
	exit(1);
      }
    }
  } else {
    if (!confirmation(FMT("INFO: Create project named `{}`?", proj_name))){
      exit(0);
    }
  }

  // make project folder
  if (!fs::exists(fs::path(full_proj_path))){
    if (!quiet) print("INFO: Making dir `{}`...\n", full_proj_path);
    if (!fs::create_directory(fs::path(full_proj_path))){
      fprint(std::cerr, "ERROR: Could not create `{}`...\n", full_proj_path);
      exit(1);
    }
  }

  // run emacs on project folder
  if (!quiet) print("INFO: Running emacs on `{}`\n", full_proj_path);
  win::run_async("runemacs", FMT("{}", full_proj_path));
  exit(0);

  return 0;
}
