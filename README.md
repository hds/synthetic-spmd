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

        -m <migration_period>
            Frequency with which a migration (for load balancing) will occur.
            Default is 0 (no migration).

        -D <disbalance_file>
            A file which specified load to be added/removed during the
            execution (see format below).

        -v
            Turns verbose mode on.


Disbalance file
---------------

The disbalance file is made up of lines of disbalance commands, the fields are
separated by tabs. The lines should be ordered by iteration. For format is:

    <iter>  edge        <dir>       <depth>     <delta>
    <iter>  alledges    <depth>     <delta>
    <iter>  row         <rowid>     <delta>
    <iter>  column      <colid>     <delta>
    <iter>  hotspot     <RxC>       <radius>    <delta>
    <iter>  chotspot    <RxC>       <radius>    <delta>
    <iter>  rectangle   <WxN>       <ExS>       <delta>
    <iter>  global      <delta>

 - `iter` is the iteration in which the disbalance will be introduced.
 - `delta` is the amount of load to be added (or removed if negative) from the affected nodes.
 - `dir` is a combination of cardinal directions N, S, E, W. These can be combined, so NE means north *and* west
 - `depth` Is the number of rows/columns from the edge to add disbalance to.
 - `rowid` is the index of a single row
 - `colid` is the index of a single column
 - `RxC` is the coordinates of a single node, rowid 'x' colid
 - `radius` is the radius of a circular hotspot, this begins with a single node for 'hotspot' or an intersection between nodes for 'chotspot' (so that it's centred).
 - `WxN` and `ExS` are the coordinates of the top-left (north-west) corner and bottom-right (south-east) corner which form a rectangle.
