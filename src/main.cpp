#define STDCPP_IMPLEMENTATION
#define USE_WINAPI
#include <stdcpp.hpp>

static bool force = false;
static std::string program;
static std::string proj_name;
static bool wants_help = false;
static bool quiet = false;

// TODO: Change every run_process() to not wait until child process
// finished when run_sync() and run_async() is implemented in stdcpp.hpp

void usage(std::ostream& os, const std::string& program){
  fprint(os, "Usage: {} [options] <proj-name>\n", program);
}

void help(const std::string& program){
  usage(std::cout, program);
  print("\n"
	"  Options:\n"
	"    /?,/h        prints this help.\n"
	"    /q           be quiet.\n"
	"    /Y           force yes on all confirmations.\n"
	"Note that you can use `-` as a prefix for flags as well.\n"
	"\n"
	"Program to make a cpp project.\n"
	"Made by Momoyon.\n");
}

bool confirmation(const std::string question, bool _default=true){
  if (force) return true;
  std::string response = "AAA";
  response = str::tolower(response);
  auto valid = [&](const std::string& str){ return str=="" || str=="y" || str=="yes" || str=="n" || str=="no"; };

  print("{} [yes/no]{{default: {}}}\n", question, _default  ? "yes" : "no");
  std::getline(std::cin, response);
  response = str::tolower(response);
  
  while (!valid(response)){
    print("Please enter yes or no\n");
    print("{} [yes/no]{{default: {}}}\n", question, _default  ? "yes" : "no");
    std::getline(std::cin, response);
    response = str::tolower(response);
  }
  ASSERT(valid(response));
  return ((_default && response=="") || response=="y" || response=="yes") ? true : false;
}

int main(int argc, char* argv[]){
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
      } else if (a == "Y"){
	force = true;
      } else {
	fprint(std::cerr, "ERROR: Unknown flag `{}{}`\n", prefix, a);
	exit(1);
      }
      
    } else {
      if (proj_name.empty())
	proj_name = a;
    }
  }

  std::string cpp_dir = get_env("CppDir");
  if (cpp_dir.empty()){
    print("WARNING: `CppDir` environment variable is not defined making the project in the current directory...\n");
  }
  if (cpp_dir[cpp_dir.size()-1] != '\\') cpp_dir += '\\';

  if (wants_help){
    help(program);
    exit(0);
  }
  
  // open the cpp_dir in emacs if no proj-name is provided
  if (proj_name.empty()){
    if (!quiet) print("INFO: Running emacs on `{}`\n", cpp_dir);


    win::wait_and_close_process(win::run_process("runemacs", cpp_dir));
    exit(0);
  }

  ASSERT(cpp_dir[cpp_dir.size()-1] == '\\');
  std::string full_proj_path = FMT("{}{}", cpp_dir, proj_name);

  // check if project already exists
  if (fs::exists(fs::path(full_proj_path))){
    if (!confirmation(FMT("INFO: Project with name `{}` already exists. Overwrite it?", proj_name))){
      exit(0);
    }
    // delete the existing folder
    if (!quiet) print("INFO: Deleting dir `{}`...\n", full_proj_path);
    if (!fs::remove_all(fs::path(full_proj_path))){
      fprint(std::cerr, "ERROR: Could not delete `{}`...\n", full_proj_path);
      exit(1);
    }
  } else {
    if (!confirmation(FMT("INFO: Create project named `{}`?", proj_name))){
      exit(0);
    }
  }
	  

  // make project folder
  if (!quiet) print("INFO: Making dir `{}`...\n", full_proj_path);
  if (!fs::create_directory(fs::path(full_proj_path))){
    fprint(std::cerr, "ERROR: Could not create `{}`...\n", full_proj_path);
    exit(1);
  }

  // run emacs on project folder
  if (!quiet) print("INFO: Running emacs on `{}`\n", full_proj_path);    
  win::wait_and_close_process(win::run_process("runemacs", FMT("{}", full_proj_path)));
  exit(0);
    
  return 0;
}
