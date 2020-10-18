#include <string>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  po::positional_options_description po_pos_desc;
  po_pos_desc.add("output", -1);
  po::options_description po_desc("Allowed options");
  
  po_desc.add_options()
    ("help,?", "show this help message")
    ("width,w", po::value<int>()->default_value(1024), "width of the output image")
    ("height,h", po::value<int>()->default_value(768), "height of the output image")
    ("output", po::value<std::string>()->required(), "output filename")
  ;
  
  po::variables_map po_vm;
  try { // read and parse command line arguments
    po::store(po::command_line_parser(argc, argv)
              .options(po_desc).positional(po_pos_desc).run(), po_vm);
    po::notify(po_vm);
  } catch (...) { // incorrect usage
    std::cout << po_desc << std::endl;
    return 0;
  }
  
  if (po_vm.count("help")) { // print help message
    std::cout << po_desc << std::endl;
    return 0;
  }
  
  const auto width = po_vm["width"].as<int>();
  const auto height = po_vm["height"].as<int>();
  const auto output = po_vm["output"].as<std::string>();
  
  std::cout << "Output: " << output << " (" << width << "x" << height << ")" << std::endl;
  
  return 0;
}
