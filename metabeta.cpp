// gpl thorfinn@binf.ku.dk
#include <getopt.h>       // for optarg, getopt_long, optind, option
#include <htslib/bgzf.h>  // for bgzf_read, bgzf_close, bgzf_open, BGZF
#include <htslib/hts.h>   // for htsFormat, hts_opt_add, htsFile, hts_opt
#include <htslib/sam.h>   // for sam_hdr_read, bam_hdr_destroy, sam_hdr_t...
#include <strings.h>      // for strcasecmp
#include <time.h>         // for clock, time, clock_t, time_t
#include <zlib.h>         // for gzprintf, gzclose, gzgets, gzopen, Z_NULL

#include <cassert>  // for assert
#include <cstdio>   // for fprintf, NULL, stderr, stdout, fopen
#include <cstdlib>  // for atoi, exit, free, atof, calloc, malloc
#include <cstring>  // for strdup, strcmp, strtok, strlen, strncpy
#include <map>      // for __map_iterator, map, operator!=, map<>::...
#include <vector>   // for vector

#include "shared.h"      // for parse_names, parse_nodes
#include "types.h"       // for int2intvec, int2int
#include "version.h"     // for METADAMAGE_VERSION

htsFormat *dingding2 = (htsFormat *)calloc(1, sizeof(htsFormat));
typedef std::map<int, char *> int2char;

int usage_splitfastas(FILE *fp){
  fprintf(fp,"options options\n");
  return 0;
}


int main_splitfastas(int argc, char **argv) {
  if (argc == 1)
    return usage_splitfastas(stderr);

    //  int MAXLENGTH = 256;
    int minLength = 35;
    int printLength = 5;
    char *refName = NULL;
    char *fname = NULL;
    int runmode = 0;  // this means one species, runmode=1 means multi species
    htsFile *fp = NULL;
    char *onam = strdup("meta");
    int nthreads = 4;
    int stopIfErrors = 1;
    // fix thesepro
    static struct option lopts[] = {
        {"fasta", 1, 0, 'f'},
        {"minlength", 1, 0, 'l'},
        {"threads", 1, 0, '@'},
        {"printlength", 1, 0, 'p'},
        {"outname", 1, 0, 'o'},
        {"help", 0, 0, '?'},
        {"runmode", 1, 0, 'r'},
	{"stopIfErrors", 1, 0, 'S'},
        {NULL, 0, NULL, 0}};

    int c;
    while ((c = getopt_long(argc, argv,
                            "f:l:p:r:o:@:S:",
                            lopts, NULL)) >= 0) {
        switch (c) {
            case 'f':
                refName = strdup(optarg);
                break;
            case 'l':
                minLength = atoi(optarg);
                break;
            case '@':
                nthreads = atoi(optarg);
                break;
            case 'p':
                printLength = atoi(optarg);
                break;
	case 'S':
                stopIfErrors = atoi(optarg);
                break;
            case 'o': {
                free(onam);
                onam = strdup(optarg);
                break;
            }
            case 'r':
                runmode = atoi(optarg);
                break;
            case '?':
                return usage_splitfastas(stdout);

            default:
                fprintf(stderr, "Never here: %s %s\n", optarg, fname);
                break;
        }
    }
    if (optind < argc)
        fname = strdup(argv[optind]);
    fprintf(stderr, "./metabeta refName: %s minLength: %d printLength: %d runmode: %d outname: %s nthreads: %d \tstopIfErrors: %d\n", refName, minLength, printLength, runmode, onam, nthreads,stopIfErrors);
    if (fname == NULL)
        return usage_splitfastas(stderr);

    if (refName) {
        char *ref = (char *)malloc(10 + strlen(refName) + 1);
        sprintf(ref, "reference=%s", refName);
        hts_opt_add((hts_opt **)&dingding2->specific, ref);
        free(ref);
    }

    if ((fp = sam_open_format(fname, "r", dingding2)) == NULL) {
        fprintf(stderr, "[%s] nonexistant file: %s\n", __FUNCTION__, fname);
        exit(0);
    }

    free(fname);
    free(onam);
    return 0;
}
int main_collate(int argc,char **argv){
  return 0;
}

int main(int argc, char **argv) {
  fprintf(stderr,"\t-> metabeta version: %s (htslib: %s) build(%s %s)\n",METABETA_VERSION,hts_version(),__DATE__,__TIME__); 
  clock_t t = clock();
  time_t t2 = time(NULL);

  if (argc == 1) {
    fprintf(stderr, "./metabeta splitfastas [other options]\n");
    fprintf(stderr, "./metabeta collate [other options]\n");
    return 0;
  }
    fprintf(stderr, "#");
    for (int i = 0; i < argc; i++)
        fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "\n");
    fflush(stderr);
    argc--;
    ++argv;
    if (!strcmp(argv[0], "splitfastas"))
        return main_splitfastas(argc, argv);
    if (!strcmp(argv[0], "collate"))
        return main_collate(argc, argv);

    free(dingding2);
    fprintf(stderr, "\t[ALL done] cpu-time used =  %.2f sec\n", (float)(clock() - t) / CLOCKS_PER_SEC);
    fprintf(stderr, "\t[ALL done] walltime used =  %.2f sec\n", (float)(time(NULL) - t2));
    return 0;
}
