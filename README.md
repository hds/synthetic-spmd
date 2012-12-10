Synthetic SPMD
==============

A synthetic SPMD application written in C using MPI.


Usage
-----

    run_mpi -n <tasks> synthetic-spmd -d <width>x<height> [options]

        -d <width>x<height> (required)
            Size of the domain making up the SMPD application,
            width * height must be equal to the number of tasks that MPI starts
            with. This flag is required.

        -u <work_units>
            Number of units of work that each process starts with. Default is 1.

        -w <min_weight>,<max_weight>
            Minimum and maximum weight for each work unit, upon start-up each
            work unit is assigned a weight from a normal distribution using
            these minimum and maximum values. Default is 1,1.

        -c <comm_size>
            Amount of data each process sends to each one of its neighbours
            during each iteration, measured in bytes. Default is 1.

        -i <iterations>
            Number of iterations the application will run for. Default is 10.

        -m <migration_freq>
            Frequency with which a migration (for load balancing) will occur.
            Default is 0 (no migration).

        -v
            Turns verbose mode on.

