/** @file distributed_table_test.cc
 *
 *  @author Dongryeol Lee (dongryel@cc.gatech.edu)
 */
#include "core/table/distributed_table.h"

bool CheckDistributedTableIntegrity(
  const core::table::DistributedTable &table_in,
  const boost::mpi::communicator &world) {
  for(int i = 0; i < world.size(); i++) {
    printf("Process %d thinks Process %d owns %d points.\n",
           world.rank(), i, table_in.local_n_entries(i));
  }
  return true;
}

int main(int argc, char *argv[]) {
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  if(world.size() <= 1) {
    std::cout << "Please specify a process number greater than 1.\n";
    exit(0);
  }
  srand(time(NULL) + world.rank());

  if(world.rank() == 0) {
    printf("%d processes are present...\n", world.size());
  }

  // Each process generates its own random data, dumps it to the file,
  // and read its own file back into its own distributed table.
  core::table::Table random_dataset;
  const int num_dimensions = 5;
  int num_points = core::math::RandInt(100, 200);
  random_dataset.Init(5, num_points);
  for(int j = 0; j < num_points; j++) {
    core::table::DensePoint point;
    random_dataset.get(j, &point);
    for(int i = 0; i < num_dimensions; i++) {
      point[i] = core::math::Random(0.1, 1.0);
    }
  }
  printf("Process %d generated %d points...\n", world.rank(), num_points);
  std::stringstream file_name_sstr;
  file_name_sstr << "random_dataset_" << world.rank() << ".csv";
  std::string file_name = file_name_sstr.str();
  random_dataset.Save(file_name);

  core::table::DistributedTable distributed_table;
  distributed_table.Init(world.rank(), file_name, &world);
  printf(
    "Process %d read in %d points...\n",
    world.rank(), distributed_table.local_n_entries());

  // Check the integrity.
  CheckDistributedTableIntegrity(distributed_table, world);

  // Each process exchanges 10 points.
  for(int i = 0; i < 10; i++) {

    // Randomly generate a target.
    int target_rank = core::math::RandInt(world.size());
    core::table::DensePoint point;
    int target_rank_table_n_entries = distributed_table.local_n_entries(
                                        target_rank);
    int target_point_id = core::math::RandInt(target_rank_table_n_entries);

    printf("Process %d requested point %d from Process %d.\n",
           world.rank(), target_point_id, target_rank);
    distributed_table.get(target_rank, target_point_id, &point);

    printf("Process %d received point %d from Process %d.\n",
           world.rank(), target_point_id, target_rank);
    point.reference().print();
  }
  printf("Process %d is all done!\n", world.rank());

  world.barrier();
  return 0;
}
