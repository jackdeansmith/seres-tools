# The Idea: 
Given an allignment of nucleotide or protein data, many inferences can be made
about phylogeny, function, or population structure. Once you make your
inference, you may question how **confident** you are.

A common method for determining your level of confidence is [bootstrap
resampling](http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29). 
In this procedure, you randomly select columns from your alignment to form a 
*resampled replicate* and run your inference technique against this new alignment. 
If out of many procedures, the replicates agree with your original conclusion, 
you can be relatively confident that it is correct. 


Bootstraping is really useful particularly because it **assumes very little** about
how the original sequences were actually created, making it widely applicable.
It does have some problem though, cheif among which is that is **compleetly
discards** the sequential nature of your data. For some techniques this isn't a
problem but for others, particularly some Hidden Markov Model based approaches,
it makes support estimates uninformative.

**SE**quentially **RE**sampled **S**upport Estimation is a difference approach
to resampling. The columns of your original data which are included in SERES replicates are not selected randomly and independantly. Instead the resampler performs a 
**random walk** over your input alignment to select the columns it emmits. This preserves the sequential nature of your data.

# Building

To make the binaries, you will need a compiler which supports c++11. Simply
execute the following:


```bash
$ git clone https://github.com/jackdeansmith/seres-tools.git
$ cd seres-tools
$ make
```

In the /bin/ directory, there should now be two binaries. `seres-resample` and
`seres-translate`.

# Usage

First, make sure that your input alignment is FASTA formatted. For this example,
we'll say it's called `alignment.fasta`. Lets say we want 100 replicates, each
1000 sites long, and using a turnaround probability of 0.001 for the resampling
procedure. We want to put these replicates in a `replicates` directory. To do this, we run:

```bash
$ mkdir replicates
$ seres-resample alignment.fasta -n100 -l1000 -b0.001 -d replicates
```

The replicates directory should now be filled with files that look like this:
```
replicate-[number].fasta
replicate-[number].walk
```

The fasta files are the resampled replicates and the walk files detail which
sites were resampled in what order. Now, you can run whatever inference method
you want on your replicate data.

Once you have inference data on your replicates, you can then use
`seres-translate` in conjunction with the walk file, to get the original
positions back. Let's say our inference method gives us 20 locations that we
care about and we write these to the comma separated file `positions`. 
To translate these back using the `replicate-1.walk` file, we run:

```bash
$ seres-translate replicate-1.walk -f positions 
```

This will write out the positions as translated back to their position in the
original alignment.

# Notes

Special thanks to [Dr. Kevin Liu](https://www.cse.msu.edu/~kjl/) who provided guidance in exploring this
idea.

