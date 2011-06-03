/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/14/2008 07:15:55 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nikolaos Vasiloglou (NV), nvasil@ieee.org
 *        Company:  Georgia Tech Fastlab-ESP Lab
 *
 * =====================================================================================
 */

#include <string>
#include "allnn.h"
#include "fastlib/fx/io.h"

using namespace std;
using namespace mlpack;
using namespace allnn;


PARAM_STRING_REQ("input_file", "File for input data", "allnn");
PARAM_STRING_REQ("output_file", "File for output data", "allnn");
int main (int argc, char *argv[]) {
	
  IO::ParseCommandLine(argc, argv);
  arma::mat data_for_tree;

  string input_file = IO::GetValue<string>("allnn/input_file");
  string output_file;

  if (IO::CheckValue("allnn/output_file"))
    output_file = IO::GetValue<string>("allnn/output_file");
  else
    output_file = "output.csv";
  
  IO::Info << "Loading file " << input_file << "..." << std::endl;
  data::Load(input_file.c_str(), data_for_tree);

  AllNN allnn(data_for_tree, fx_root);
  
  arma::Col<index_t> resulting_neighbors_tree;
  arma::vec resulting_distances_tree;

  IO::Info << "Computing neighbors...";

  allnn.ComputeNeighbors(resulting_distances_tree, resulting_neighbors_tree);

  IO::Info << "Saving results to " << output_file << "..." << std::endl;

  data::Save(output_file.c_str(), resulting_neighbors_tree, resulting_distances_tree); 
}
