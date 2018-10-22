/* reduce.c */
/************/

/* This is a modified version of the program 'reduce.c' by N. Robertson,
   D. P. Sanders, P. D. Seymour and R. Thomas. Modifications by
   J.P. Steinberger, June 2008.
   Modified lines are ended by a comment of the form `// jps'. 
   The original header follows below: */

/* This is part I of two programs that serve as supplements to the paper `The
 * Four-Colour Theorem' by N. Robertson, D. P. Sanders, P. D. Seymour and R.
 * Thomas. Please refer to the manuscript `Reducibility in the Four-Color
 * Theorem' by the same authors for a description of this program */

/* Copyright 1995 by N. Robertson, D.P. Sanders, P.D. Seymour and R. Thomas  */
/* Permission to use for the purpose of scholarly research is hereby granted */

/* Version 1,  8 May 1995 */

#define VERTS   30      /* max number of vertices in a free completion + 1 */ // jps
#define DEG     13      /* max degree of a vertex in a free completion + 1 */
                        /* must be at least 13 because of row 0            */
#define EDGES   70      /* max number of edges in a free completion + 1    */ // jps
#define MAXRING 16      /* max ring-size */ // jps

#include <stdio.h>
#include <stdlib.h>

// `tp_confmat` is a type that probably stands for "Configuration Matrix".
// Rather than doing something sensible and being a `struct`, this is just a
// 2D array for 30x13 = 390 `long`s.
//
// On my machine, `sizeof(long) == 8`, but I don't know what originally was
// intended. It's entirely possible `long` was intended to mean 32 bits
// (`sizeof(long) == 4`).
//
// In addition to containing the graph of the configuration (stored as an
// adjacency list), this also contains header information like the number of
// vertices, the ring size, the number of colourings that extend to the
// configuration, etc. These are stored in vertex 0, because the graph vertices
// are numbered from 1. (For example, if tp_confmat A, A[0][0] is the number of
// vertices in the configuration A)
//
// The ring vertices are stored first. (That is, A[i] is a ring vertex if
// i > 0 && i <= A[0][1])
typedef long tp_confmat[VERTS][DEG];

/* Mock-up for how the configuration matrix would look if it wasn't just an
 * array of 390 `long`s.
 *
 * !!IMPORTANT NOTE!! If I do decide to go this route, extreme care must be taken
 * regarding indices. There are many places in this code where arrays start at 1,
 * but these structures start at 0.
 * Possibly, I could use a union to be able to refer to both the `long`s as a
 * header and the contents of vertex zero.
 *
 * And of course, `tp_confmat` is a pointer to 390 `long`s. The struct is a
 * value. (some things need to take it by pointer)

 * Hmm. It appears that this `Contract` (obtained from ReadConf) is not the same
 * as the `contract` passed to `findangles()` or related. That `contract` is
 * `long contract[EDGES + 1]`.
typedef struct ContractDef {
    // How many entries this contract has. Range: 0, 1, 2, 3, 4.
    long size;
    // 2*size `long` values. Equivalently, `size` edges.
    long entries[8];
} Contract;

typedef struct Vertex {
    // How many neighbors this vertex has.
    long degree;

    // Minus one because DEG is the number of vertices in a free completion plus
    // one.
    long neighbors[DEG - 1];
} Vertex;

typedef struct ConfigurationMatrix {
    // The number of vertices in this configuration.
    long num_vertices;
    // The ring size of this configuration.
    long ring_size;
    // The number of colourings that can be extended to this configuration.
    long num_extendable_colourings;
    // The "max cons subset". I have no idea what that means.
    long max_cons_subset;

    // The contract for this configuration.
    ContractDef contract;

    // The adjacency matrix.
    // Minus one because VERTS is the number of vertices in a free completion
    // plus one. (The plus one is used to store the header)
    //
    // DEG is not minus one, because matrix[i][0] is the degree of vertex `i`.
    // long matrix[VERTS - 1][DEG];
    //
    // The first `self.ring_size` vertices compose the outer ring of this
    // configuration.
    Vertex matrix[VERTS - 1];
} ConfigurationMatrix;

*/

typedef long tp_angle[EDGES][5];

// `tp_edgeno` probably stands for "edge numbering".
// Why are the dimensions for this type.
typedef long tp_edgeno[EDGES][EDGES];

/* function prototypes */
void testmatch(long, char *, long[], char *, long);
void augment(long, long[], long, long **, long[MAXRING+1][MAXRING+1][4], char *, char *, long *, long, long, long, char *, long *, long); // jps
void checkreality(long, long **, char *, char *, long *, long, long, long, char *, long *, long);
long stillreal(long, long[], long, char *, long);
long updatelive(char *, long, long *);
void strip(tp_confmat, tp_edgeno);
long ininterval(long[], long[]);
void findangles(tp_confmat, tp_angle, tp_angle, tp_angle, long[]);
long findlive(char *, long, tp_angle, long[], long);
void checkcontract(char *, long, tp_angle, tp_angle, long[], long[]);
void printstatus(long, long, long, long);
void record(long[], long[], long, long[][5], char *, long *, long);
long inlive(long[], long[], long, char *, long);
long ReadConf(tp_confmat, FILE *, long *);
void ReadErr(int, char[]);

// static long simatchnumber[] = {
    // 0L, 0L, 1L, 3L, 10L, 30L, 95L, 301L, 980L, 3228L, 10797L, 36487L, 124542L, 428506L, 1485003L
// };

static long simatchnumber[] = {
    0L, 0L, 1L, 3L, 10L, 30L, 95L, 301L, 980L, 3228L, 10797L, 36487L, 124542L, 428506L, 1485003L, 5178161L,  18155816L
}; // jps



int
main(int argc, char *argv[])
{
    long ring, nlive, ncodes, i, nchar, count, power[MAXRING + 2], contract[EDGES + 1]; // jps
    tp_angle angle, diffangle, sameangle;
    tp_confmat graph;
    char *live, *real, *s;
    FILE *fp;
    if (argc < 2) {
        s = "unavoidable.conf";
    } else {
        s = argv[1];
    }

    fp = fopen(s, "r");
    if (fp == NULL) {
        (void) printf("Can't open %s\n", s);
        exit(1);
    }

    power[1] = 1;
    for (i = 2; i <= MAXRING + 1; i++) { // jps
        /* power[i] = 3^(i-1) for i>0 */
        power[i] = 3 * power[i - 1];
    }

    ncodes = (power[MAXRING] + 1) / 2;   /* max number of codes */
    live = (char *) malloc(ncodes * sizeof(char));
    nchar = simatchnumber[MAXRING] / 8 + 2;
    real = (char *) malloc(nchar * sizeof(char));
    if (live == NULL || real == NULL) {
        i = (ncodes + nchar) * sizeof(char);
        (void) printf("Not enough memory. %ld Kbytes needed.\n", i / 1024 + 1);
        exit(44);
    }

    for (count = 0; !ReadConf(graph, fp, NULL); count++) {

        // Plus one to start counting configurations from 1.
        printf("Checking configuration number %ld\n", count + 1);

        /* "findangles" fills in the arrays "angle", "diffangle", "sameangle" and
         * "contract" from the input "graph". "angle" will be used to compute
         * which colourings of the ring edges extend to the configuration; the
         * others will not be used unless a contract is specified, and if so
         * they will be used in "checkcontract" below to verify that the
         * contract is correct. */
        findangles(graph, angle, diffangle, sameangle, contract);

        /* ring-size */
        ring = graph[0][1];
        if (ring > MAXRING) {
            (void) printf("Ring-size bigger than %d\n", MAXRING);
            exit(43);
        }

        /* number of codes of colorings of R */
        ncodes = (power[ring] + 1) / 2;
        for (i = 0; i < ncodes; i++) {
            live[i] = 1;
        }
        /* "findlive" computes {\cal C}_0 and stores in live */
        nlive = findlive(live, ncodes, angle, power, graph[0][2]);

        /* "real" will be an array of characters, and each bit of each
         * character will correspond to a balanced signed matching. At this
         * stage all the bits are set = 1. */
        nchar = simatchnumber[ring] / 8 + 1;
        for (i = 0; i <= nchar; i++) {
            real[i] = (char) 255;
        }

        /* computes {\cal C}_{i+1} from {\cal C}_i, updates "live" */
        do {
            /* computes {\cal M}_{i+1} from {\cal M}_i, updates the bits of "real" */
            testmatch(ring, real, power, live, nchar);
        } while (updatelive(live, ncodes, &nlive));

        /* This verifies that the set claimed to be a contract for the
         * configuration really is. */
        checkcontract(live, nlive, diffangle, sameangle, contract, power);
    }

    (void) fclose(fp);
    free(live);
    free(real);
    (void) printf("Reducibility of %ld configurations verified\n", count);
    return (0);
}


/* This generates all balanced signed matchings, and for each one, tests
 * whether all associated colourings belong to "live". It writes the answers
 * in the bits of the characters of "real". */
void
testmatch(long ring, char *real, long power[], char *live, long nchar)
{
    long a, b, n, interval[10], *weight[8], nreal;
    long matchweight[MAXRING + 1][MAXRING + 1][4], *mw, realterm; // jps
    char bit;

    /* "nreal" will be the number of balanced signed matchings such that all
     * associated colourings belong to "live"; ie the total number of nonzero
     * bits in the entries of "real" */
    nreal = 0;
    bit = 1;
    realterm = 0;

    /* First, it generates the matchings not incident with the last ring edge */
    for (a = 2; a <= ring; a++) {
        for (b = 1; b < a; b++) {
            mw = matchweight[a][b];
            mw[0] = 2 * (power[a] + power[b]);
            mw[1] = 2 * (power[a] - power[b]);
            mw[2] = power[a] + power[b];
            mw[3] = power[a] - power[b];
        }
    }

    for (a = 2; a < ring; a++) {
        for (b = 1; b < a; b++) {
            n = 0;
            weight[1] = matchweight[a][b];
            if (b >= 3) {
                n = 1;
                interval[1] = 1;
                interval[2] = b - 1;
            }
            if (a >= b + 3) {
                n++;
                interval[2 * n - 1] = b + 1;
                interval[2 * n] = a - 1;
            }
            augment(n, interval, (long) 1, weight, matchweight, live, real, &nreal, ring, (long) 0, (long) 0, &bit, &realterm, nchar);
        }
    }

    /* now, the matchings using an edge incident with "ring" */
    for (a = 2; a <= ring; a++) {
        for (b = 1; b < a; b++) {
            mw = matchweight[a][b];
            mw[0] = power[a] + power[b];
            mw[1] = power[a] - power[b];
            mw[2] = -power[a] - power[b];
            mw[3] = -power[a] - 2 * power[b];
        }
    }

    for (b = 1; b < ring; b++) {
        n = 0;
        weight[1] = matchweight[ring][b];
        if (b >= 3) {
            n = 1;
            interval[1] = 1;
            interval[2] = b - 1;
        }
        if (ring >= b + 3) {
            n++;
            interval[2 * n - 1] = b + 1;
            interval[2 * n] = ring - 1;
        }
        augment(n, interval, (long) 1, weight, matchweight, live, real, &nreal, ring, (power[ring + 1] - 1) / 2, (long) 1, &bit, &realterm, nchar);
    }

    (void) printf("               %ld\n", nreal);
    (void) fflush(stdout);
}

/* Finds all matchings such that every match is from one of the given
 * intervals. (The intervals should be disjoint, and ordered with smallest
 * first, and lower end given first.) For each such matching it examines all
 * signings of it, and adjusts the corresponding entries in "real" and
 * "live". */
void
augment(
    long n,
    long interval[10],
    long depth,
    long *weight[8],
    long matchweight[MAXRING + 1][MAXRING + 1][4],
    char *live,
    char *real,
    long *pnreal,
    long ring,
    long basecol,
    long on,
    char *pbit,
    long *prealterm,
    long nchar)
{
    long h, i, j, r, newinterval[10], newn, lower, upper;

    checkreality(depth, weight, live, real, pnreal, ring, basecol, on, pbit, prealterm, nchar);
    depth++;
    for (r = 1; r <= n; r++) {
        lower = interval[2 * r - 1];
        upper = interval[2 * r];
        for (i = lower + 1; i <= upper; i++) {
            for (j = lower; j < i; j++) {
                weight[depth] = matchweight[i][j];
                for (h = 1; h < 2 * r - 1; h++)
                    newinterval[h] = interval[h];
                newn = r - 1;
                if (j > lower + 1) {
                    newn++;
                    newinterval[h++] = lower;
                    newinterval[h++] = j - 1;
                }
                if (i > j + 1) {
                    newn++;
                    newinterval[h++] = j + 1;
                    newinterval[h++] = i - 1;
                }
                augment(newn, newinterval, depth, weight, matchweight, live,
                        real, pnreal, ring, basecol, on, pbit, prealterm, nchar);
            }
        }
    }
}


/* For a given matching M, it runs through all signings, and checks which of
 * them have the property that all associated colourings belong to "live". It
 * writes the answers into bits of "real", starting at the point specified by
 * "bit" and "realterm". "basecol" is for convenience in computing the
 * associated colourings; it is zero for matchings not incident with "ring".
 * "on" is nonzero iff the matching is incident with "ring". */
void
checkreality(
    long depth,
    long *weight[8],
    char *live,
    char *real,
    long *pnreal,
    long ring,
    long basecol,
    long on,
    char *pbit,
    long *prealterm,
    long nchar)
{
    long i, k, nbits, choice[8], col, parity;
    unsigned long left;

    nbits = 1 << (depth - 1);

    /* k will run through all subsets of M minus the first match */
    for (k = 0; k < nbits; k++, *pbit <<= 1) {
        if (!*pbit) {
            *pbit = 1;
            ++(*prealterm);
            if (*prealterm > nchar) {
                (void) printf("More than %ld entries in real are needed\n", nchar + 1);
                exit(32);
            }
        }

        if (!(*pbit & real[*prealterm]))
            continue;

        col = basecol;
        parity = ring & 1;
        for (i = 1, left = k; i < depth; i++, left >>= 1) {
            if (left & 1) { /* i.e. if a_i=1, where k=a_1+2a_2+4a_3+... */
               parity ^= 1;
               choice[i] = weight[i][1];
               col += weight[i][3];
            } else {
               choice[i] = weight[i][0];
               col += weight[i][2];
            }
        }

        if (parity) {
            choice[depth] = weight[depth][1];
            col += weight[depth][3];
        } else {
            choice[depth] = weight[depth][0];
            col += weight[depth][2];
        }

        if (!stillreal(col, choice, depth, live, on)) {
            real[*prealterm] ^= *pbit;
        } else {
            (*pnreal)++;
        }
    }
}


/* Given a signed matching, this checks if all associated colourings are in
 * "live", and, if so, records that fact on the bits of the corresponding
 * entries of "live". */
long
stillreal(long col, long choice[8], long depth, char *live, long on)
{
    long sum[128], mark, i, j, twopower, b, c; // jps
    long twisted[128], ntwisted, untwisted[128], nuntwisted; // jps

    ntwisted = nuntwisted = 0;
    if (col < 0) {
        if (!live[-col]) {
            return (long) 0;
        }

        twisted[ntwisted++] = -col;
        sum[0] = col;
    } else {
        if (!live[col]) {
            return (long) 0;
        }

        untwisted[nuntwisted++] = sum[0] = col;
    }

    for (i = 2, twopower = 1, mark = 1; i <= depth; i++, twopower <<= 1) {
        c = choice[i];
        for (j = 0; j < twopower; j++, mark++) {
            b = sum[j] - c;
            if (b < 0) {
                if (!live[-b]) {
                    return (long) 0;
                }

                twisted[ntwisted++] = -b;
                sum[mark] = b;
            } else {
                if (!live[b]) {
                    return (long) 0;
                }

                untwisted[nuntwisted++] = sum[mark] = b;
            }
        }
    }

    /* Now we know that every coloring that theta-fits M has its code in
     * "live". We mark the corresponding entry of "live" by theta, that is,
     * set its second, third or fourth bit to 1 */

    if (on) {
        for (i = 0; i < ntwisted; i++) {
            live[twisted[i]] |= 8;
        }

        for (i = 0; i < nuntwisted; i++) {
            live[untwisted[i]] |= 4;
        }
    } else {
        for (i = 0; i < ntwisted; i++) {
            live[twisted[i]] |= 2;
        }

        for (i = 0; i < nuntwisted; i++) {
            live[untwisted[i]] |= 2;
        }
    }

    return (long) 1;
}


/* runs through "live" to see which colourings still have `real' signed
 * matchings sitting on all three pairs of colour classes, and updates "live"
 * accordingly; returns 1 if nlive got smaller and stayed >0, and 0 otherwise */
long
updatelive(char *live, long ncols, long *p)
{
    long i, nlive, newnlive;

    nlive = *p;
    newnlive = 0;

    if (live[0] > 1) {
        live[0] = (char) 15;
    }

    for (i = 0; i < ncols; i++) {
        if (live[i] != 15) {
            live[i] = 0;
        } else {
            newnlive++;
            live[i] = 1;
        }
    }

    *p = newnlive;
    (void) printf("            %9ld", newnlive);
    (void) fflush(stdout);

    if ((newnlive < nlive) && (newnlive > 0)) {
        return (long) 1;
    }

    if (!newnlive) {
        (void) printf("\n\n\n                  ***  D-reducible  ***\n\n");
    } else {
        (void) printf("\n\n\n                ***  Not D-reducible  ***\n");
    }

    return (long) 0;
}

/* Numbers edges from 1 up, so that each edge has as many later edges in
 * triangles as possible; the ring edges are first.  edgeno[u][v] will be the
 * number of the edge with ends u,v if there is such an edge and 0 otherwise. */
void
strip(tp_confmat graph, tp_edgeno edgeno)
{
    long d, h, u, v, w, x, verts, ring, term, maxint, maxes, max[VERTS];
    long inter, maxdeg, best, first, previous, *grav, done[VERTS];

    for (u = 1; u < VERTS; u++) {
        for (v = 1; v < VERTS; v++) {
            edgeno[u][v] = 0;
        }
    }

    verts = graph[0][0];
    ring = graph[0][1];
    for (v = 1; v <= ring; v++) {
        u = (v > 1) ? v - 1 : ring;
        edgeno[u][v] = v;
        edgeno[v][u] = v;
    }

    for (v = 1; v <= verts; v++) {
        done[v] = 0;
    }

    term = 3 * (verts - 1) - ring;
    for (x = ring + 1; x <= verts; x++) {
        /* First we find all vertices from the interior that meet the "done"
         * vertices in an interval, and write them in max[1] .. max[maxes] */
        maxint = 0;
        maxes = 0;
        for (v = ring + 1; v <= verts; v++) {
            if (done[v]) {
                continue;
            }

            inter = ininterval(graph[v], done);
            if (inter > maxint) {
                maxint = inter;
                maxes = 1;
                max[1] = v;
            } else if (inter == maxint) {
                max[++maxes] = v;
            }
        }   /* for v bracket */

        /* From the terms in max we choose the one of maximum degree */
        maxdeg = 0;

        for (h = 1; h <= maxes; h++) {
            d = graph[max[h]][0];
            if (d > maxdeg) {
                maxdeg = d;
                best = max[h];
            }
        }
        /* So now, the vertex "best" will be the next vertex to be done */

        grav = graph[best];
        d = grav[0];
        first = 1;
        previous = done[grav[d]];
        while ((previous) || (!done[grav[first]])) {
            previous = done[grav[first++]];
            if (first > d) {
                first = 1;
                break;
            }
        }

        for (h = first; done[grav[h]]; h++) {
            edgeno[best][grav[h]] = term;
            edgeno[grav[h]][best] = term;
            term--;
            if (h == d) {
                if (first == 1)
                    break;
                h = 0;
            }
        }
        done[best] = 1;
    }   /* for x bracket */
    /* This eventually lists all the internal edges of the configuration */

    /* Now we must list the edges between the interior and the ring */
    for (x = 1; x <= ring; x++) {
        maxint = 0;
        for (v = 1; v <= ring; v++) {
            if (done[v]) {
                continue;
            }
            u = (v > 1) ? v - 1 : ring;
            w = (v < ring) ? v + 1 : 1;
            inter = 3 * graph[v][0] + 4 * (done[u] + done[w]);
            if (inter > maxint) {
                maxint = inter;
                best = v;
            }
        }   /* for v bracket */

        grav = graph[best];
        u = (best > 1) ? best - 1 : ring;
        if (done[u]) {
            for (h = grav[0] - 1; h >= 2; h--) {
                edgeno[best][grav[h]] = term;
                edgeno[grav[h]][best] = term;
                term--;
            }
        } else {
            for (h = 2; h < grav[0]; h++) {
                edgeno[best][grav[h]] = term;
                edgeno[grav[h]][best] = term;
                term--;
            }
        }
        done[best] = 1;
    }   /* for x bracket */
}


/* if grav meets the done vertices in an interval of length >=1, it returns
 * the length of the interval, and otherwise returns 0 */
long
ininterval(long grav[], long done[])
{
    long d, j, first, last, worried, length;

    d = grav[0];

    // Note: This loop is not actually useless.
    // This exists because of the side effect 'first++'.
    // It would be clearer if it had been written:
    // first = 1;
    // while ((first < d) && !done[grav[first]]) { first++; }
    for (first = 1; (first < d) && (!done[grav[first]]); first++)
        ;

    if (first == d) {
        return (done[grav[d]]);
    }

    // Note: A similar situation occurs here.
    for (last = first; (last < d) && (done[grav[last + 1]]); last++)
        ;

    length = last - first + 1;
    if (last == d) {
        return (length);
    }

    if (first > 1) {
        for (j = last + 2; j <= d; j++) {
            if (done[grav[j]]) {
                return (long) 0;
            }
        }

        return length;
    }

    worried = 0;
    for (j = last + 2; j <= d; j++) {
        if (done[grav[j]]) {
            length++;
            worried = 1;
        } else if (worried) {
            return (long) 0;
        }
    }

    return length;
}

/* writes into angle[i] all edges with number >i on a common triangle T say
 * with edge i; and if there is a contract X given, and i is not in X, writes
 * into diffangle[i] all such edges such that no edge of T is in X, and
 * writes into sameangle[i] all such edges not in X so that the third edge of
 * T is in X. Sets contract[i] to 1 if edge number i is in X and to zero
 * otherwise, checks that X is sparse, and if |X|=4 checks that X has a triad */
void
findangles(tp_confmat graph, tp_angle angle, tp_angle diffangle, tp_angle sameangle, long contract[])
{
    long a, b, c, h, i, j, u, v, w, edges;
    tp_edgeno edgeno;
    long neighbour[VERTS];

    edges = 3 * graph[0][0] - 3 - graph[0][1];
    if (edges >= EDGES) {
        (void) printf("Configuration has more than %d edges\n", EDGES - 1);
        exit(20);
    }

    strip(graph, edgeno);

    for (i = 0; i < EDGES + 1; i++) {
        contract[i] = 0;
    }

    contract[0] = graph[0][4];  /* number of edges in contract */
    if (contract[0] < 0 || contract[0] > 4) {
        (void) printf("         ***  ERROR: INVALID CONTRACT  ***\n\n");
        exit(27);
    }

    for (i = 5; i <= 2 * contract[0] + 4; i++) {
        if (graph[0][i] < 1 || graph[0][i] > graph[0][0]) {
            (void) printf("         ***  ERROR: ILLEGAL CONTRACT  ***\n\n");
            exit(29);
        }
    }

    contract[EDGES] = graph[0][3];
    for (i = 1; i <= contract[0]; i++) {
        u = graph[0][2 * i + 3];
        v = graph[0][2 * i + 4];
        if (edgeno[u][v] < 1) {
            (void) printf("         ***  ERROR: CONTRACT CONTAINS NON-EDGE  ***\n\n");
            exit(29);
        }
        contract[edgeno[u][v]] = 1;
    }

    for (i = 1; i <= graph[0][1]; i++) {
        if (contract[i]) {
            (void) printf("         ***  ERROR: CONTRACT IS NOT SPARSE  ***\n\n");
            exit(21);
        }
    }

    for (i = 1; i <= edges; i++) {
        diffangle[i][0] = sameangle[i][0] = angle[i][0] = 0;
    }

    diffangle[0][0] = angle[0][0] = graph[0][0];
    diffangle[0][1] = angle[0][1] = graph[0][1];
    diffangle[0][2] = angle[0][2] = edges;

    for (v = 1; v <= graph[0][0]; v++) {
        for (h = 1; h <= graph[v][0]; h++) {
            if ((v <= graph[0][1]) && (h == graph[v][0])) {
                continue;
            }

            i = (h < graph[v][0]) ? h + 1 : 1;
            u = graph[v][h];
            w = graph[v][i];
            a = edgeno[v][w];
            b = edgeno[u][w];
            c = edgeno[u][v];

            if (contract[a] && contract[b]) {
                (void) printf("         ***  ERROR: CONTRACT IS NOT SPARSE  ***\n\n");
                exit(22);
            }

            if (a > c) {
                angle[c][++angle[c][0]] = a;
                if ((!contract[a]) && (!contract[b]) && (!contract[c])) {
                    diffangle[c][++diffangle[c][0]] = a;
                }

                if (contract[b]) {
                    sameangle[c][++sameangle[c][0]] = a;
                }
            }

            if (b > c) {
                angle[c][++angle[c][0]] = b;
                if ((!contract[a]) && (!contract[b]) && (!contract[c])) {
                    diffangle[c][++diffangle[c][0]] = b;
                }

                if (contract[a]) {
                    sameangle[c][++sameangle[c][0]] = b;
                }
            }
        }
    }

    /* checking that there is a triad */
    if (contract[0] < 4) {
        return;
    }

    for (v = graph[0][1] + 1; v <= graph[0][0]; v++) {
        /* v is a candidate triad */
        for (a = 0, i = 1; i <= graph[v][0]; i++) {
            u = graph[v][i];
            for (j = 5; j <= 12; j++) {
                if (u == graph[0][j]) {
                    a++;
                    break;
                }
            }
        }

        if (a < 3) {
            continue;
        }
        if (graph[v][0] >= 6) {
            return;
        }
        for (u = 1; u <= graph[0][0]; u++) {
            neighbour[u] = 0;
        }
        for (i = 1; i <= graph[v][0]; i++) {
            neighbour[graph[v][i]] = 1;
        }
        for (j = 5; j <= 12; j++) {
            if (!neighbour[graph[0][j]]) {
                return;
            }
        }
    }

    (void) printf("         ***  ERROR: CONTRACT HAS NO TRIAD  ***\n\n");
    exit(28);
}


/* computes {\cal C}_0 and stores it in live. That is, computes codes of
 * colorings of the ring that are not restrictions of tri-colorings of the
 * free extension. Returns the number of such codes */
long
findlive(char *live, long ncodes, tp_angle angle, long power[], long extentclaim)
{
    long j, c[EDGES], i, u, *am;
    long edges, ring, extent, bigno;
    long forbidden[EDGES];  /* called F in the notes */

    ring = angle[0][1];
    edges = angle[0][2];
    bigno = (power[ring + 1] - 1) / 2;  /* needed in "record" */
    c[edges] = 1;
    j = edges - 1;
    c[j] = 2;
    forbidden[j] = 5;

    for (extent = 0;;) {
        while (forbidden[j] & c[j]) {
            c[j] <<= 1;
            while (c[j] & 8) {
                if (j >= edges - 1) {
                    printstatus(ring, ncodes, extent, extentclaim);
                    return (ncodes - extent);
                }
                c[++j] <<= 1;
            }
        }

        if (j == ring + 1) {
            record(c, power, ring, angle, live, &extent, bigno);
            c[j] <<= 1;

            while (c[j] & 8) {
                if (j >= edges - 1) {
                    printstatus(ring, ncodes, extent, extentclaim);
                    return ncodes - extent;
                }
                c[++j] <<= 1;
            }
        } else {
            am = angle[--j];
            c[j] = 1;
            for (u = 0, i = 1; i <= am[0]; i++)
                u |= c[am[i]];
            forbidden[j] = u;
        }
    }
}

/* checks that no colouring in live is the restriction to E(R) of a
 * tri-coloring of the free extension modulo the specified contract */
void
checkcontract(char *live, long nlive, tp_angle diffangle, tp_angle sameangle, long contract[EDGES + 1], long power[])
{
    long j, c[EDGES], i, u, *dm, *sm;
    long ring, bigno;
    long forbidden[EDGES];  /* called F in the notes */
    long start;             /* called s in the notes */

    if (!nlive) {
        if (!contract[0]) {
            (void) printf("\n");
            return;
        } else {
            (void) printf("         ***  ERROR: CONTRACT PROPOSED  ***\n\n");
            exit(23);
        }
    }

    if (!contract[0]) {
        (void) printf("       ***  ERROR: NO CONTRACT PROPOSED  ***\n\n");
        exit(24);
    }

    if (nlive != contract[EDGES]) {
        (void) printf("       ***  ERROR: DISCREPANCY IN EXTERIOR SIZE  ***\n\n");
        exit(25);
    }

    ring = diffangle[0][1];
    bigno = (power[ring + 1] - 1) / 2;  /* needed in "inlive" */
    start = diffangle[0][2];
    while (contract[start]) {
        start--;
    }

    c[start] = 1;
    j = start;
    while (contract[--j]) {
        ;
    }

    dm = diffangle[j];
    sm = sameangle[j];
    c[j] = 1;
    for (u = 4, i = 1; i <= dm[0]; i++) {
        u |= c[dm[i]];
    }
    for (i = 1; i <= sm[0]; i++) {
        u |= ~c[sm[i]];
    }
    forbidden[j] = u;

    for (;;) {
        while (forbidden[j] & c[j]) {
            c[j] <<= 1;
            while (c[j] & 8) {
                while (contract[++j]) {
                    ;
                }
                if (j >= start) {
                    (void) printf("               ***  Contract confirmed  ***\n\n");
                    return;
                }
                c[j] <<= 1;
            }
        }

        if (j == 1) {
            if (inlive(c, power, ring, live, bigno)) {
                (void) printf("       ***  ERROR: INPUT CONTRACT IS INCORRECT  ***\n\n");
                exit(26);
            }
            c[j] <<= 1;
            while (c[j] & 8) {
                while (contract[++j]) {
                    ;
                }
                if (j >= start) {
                    (void) printf("               ***  Contract confirmed  ***\n\n");
                    return;
                }
                c[j] <<= 1;
            }
            continue;
        }

        while (contract[--j]) {
            ;
        }

        dm = diffangle[j];
        sm = sameangle[j];
        c[j] = 1;
        for (u = 0, i = 1; i <= dm[0]; i++) {
            u |= c[dm[i]];
        }
        for (i = 1; i <= sm[0]; i++) {
            u |= ~c[sm[i]];
        }
        forbidden[j] = u;
    }
}

void
printstatus(long ring, long totalcols, long extent, long extentclaim)
{
    (void) printf("\n\n   This has ring-size %ld, so there are %ld colourings total,\n",ring, totalcols);
    (void) printf("   and %ld balanced signed matchings.\n",simatchnumber[ring]);

    (void) printf("\n   There are %ld colourings that extend to the configuration.", extent);
    if (extent != extentclaim) {
        (void) printf("\n   *** ERROR: DISCREPANCY IN NUMBER OF EXTENDING COLOURINGS ***\n");
        exit(31);
    }
    (void) printf("\n\n            remaining               remaining balanced\n");
    (void) printf("           colourings               signed matchings\n");
    (void) printf("\n              %7ld", totalcols - extent);
    (void) fflush(stdout);
}

/* Given a colouring specified by a 1,2,4-valued function "col", it computes
 * the corresponding number, checks if it is in live, and if so removes it. */
void
record(long col[], long power[], long ring, long angle[][5], char *live, long *p, long bigno)
{
    long weight[5], colno, sum, i, min, max, w;

    for (i = 1; i < 5; i++) {
        weight[i] = 0;
    }

    for (i = 1; i <= ring; i++) {
        sum = 7 - col[angle[i][1]] - col[angle[i][2]];
        weight[sum] += power[i];
    }

    min = max = weight[4];
    for (i = 1; i <= 2; i++) {
        w = weight[i];
        if (w < min) {
            min = w;
        } else if (w > max) {
            max = w;
        }
    }

    colno = bigno - 2 * min - max;
    if (live[colno]) {
        (*p)++;
        live[colno] = 0;
    }
}

/* Same as "record" above, except now it returns whether the colouring is in
 * live, and does not change live. */
long
inlive(long col[], long power[], long ring, char *live, long bigno)
{
    long weight[5], colno, i, min, max, w;

    for (i = 1; i < 5; i++) {
        weight[i] = 0;
    }

    for (i = 1; i <= ring; i++) {
        weight[col[i]] += power[i];
    }

    min = max = weight[4];
    for (i = 1; i <= 2; i++) {
        w = weight[i];
        if (w < min) {
            min = w;
        } else if (w > max) {
            max = w;
        }
    }

    colno = bigno - 2 * min - max;
    return (long) live[colno];
}


/* Reads one graph from file F and stores in A, if C!=NULL puts coordinates
 * there. If successful returns 0, on end of file returns 1, if error exits. */
long
ReadConf(tp_confmat A, FILE *F, long *C)
{
    char S[256], *t, name[256];
    long d, i, j, k, n, r, a, p;

    // Get the name of the configuration (max 255 characters)?
    name[0] = '\0';
    t = name;
    // This loops over blank lines until it find the a non-blank character.
    // It then stores that line in `name`.
    while (*t == '\0' || *t == '\n') {
        // Read one line into `name`. If this fails (because EOF or something),
        // return 1.
        // Note: this line may not actually contain a name; it might be blank.
        if (fgets(name, sizeof(name), F) == NULL) {
            return (long) 1;
        }

        // Search through `name` for a character that is not whitespace. If the
        // end of the line is encountered, also break.
        // On the next loop iteration, if `t` points to whitespace, go to the next
        // line, otherwise, continue past this loop, without modifying `name`
        // again.
        for (t = name; *t == ' ' || *t == '\t'; t++)
            ;
    }

    // Read one line from the file into `S` (maximum 255 characters)
    (void) fgets(S, sizeof(S), F);

    /* No verts, ringsize, no extendable colourings, max cons subset */
    // Translation: Read from `S` the:
    // * number of vertices in the configuration
    // * ring size of the configuration
    // * number of ring colourings that can be extended to the configuration
    // * number of ring colourings in the maximal consistent subset
    // and then store them in A[0][0], A[0][1], A[0][2], and A[0][3], respectively.
    //
    // It appears that A[0] serves as sort of like a header/metadata?
    // The first vertex is stored in A[1][j].
    if (sscanf(S, "%ld%ld%ld%ld", &A[0][0], &A[0][1], &A[0][2], &A[0][3]) != 4) {
        (void) printf("Error on line 2 while reading %s\n", name);
        exit(11);
    }

    n = A[0][0];
    r = A[0][1];
    if (n >= VERTS) {
        (void) printf("%s has more than %d vertices\n", name, VERTS - 1);
        exit(17);
    }

    // Read the configuration's contract from the next line.
    // This line consists of 1, 3, 5, 7, or 9 numbers.
    // The first number is the number of contract values.
    // The rest of the line consists of twice as many numbers.
    // For example, the line
    //  0
    // is a configuration without a contract.
    // The line
    //  1   12 10
    // is a configuration with two contract numbers (I don't know how to
    // interpret the contract.)
    // The line
    //  4   1 9 3 9 5 11 7 11
    // is a configuration with eight contract numbers. (The maximum number of
    // contract numbers)
    (void) fgets(S, sizeof(S), F);  /* Contract */
    i = sscanf(S, "%ld%ld%ld%ld%ld%ld%ld%ld%ld", &A[0][4], &A[0][5], &A[0][6], &A[0][7], &A[0][8], &A[0][9], &A[0][10], &A[0][11], &A[0][12]);
    if (2 * A[0][4] + 1 != i) {
       (void) printf("Error on line 3 while reading %s\n", name);
       exit(13);
    }

    /* Reading adjacency list */
    // Read the vertices and edges (specified as an adjacency list).
    for (i = 1; i <= n; i++) {
        // Each line is a vertex.
        //
        // For example, the line
        //  7  5    2  8  9 10  1
        // means that this is vertex number 7, and it has five neighbors:
        // vertex 2, vertex 8, vertex 9, vertex 10, and vertex 1.
        (void) fgets(S, sizeof(S), F);

        // Read the vertex number and the degree of this vertex.
        // The degree of vertex `i` is stored in A[i][0].
        if (sscanf(S, "%ld%ld", &j, &A[i][0]) != 2 || i != j) {
            (void) printf("Error while reading vertex %ld of %s\n", i, name);
            exit(14);
        }
        if (A[i][0] >= DEG) {
            (void) printf("Vertex degree larger than %d in %s\n", DEG - 1, name);
            exit(14);
        }

        // I think this skips over the two numbers that were just parsed.
        for (t = S; *t < '0' || *t > '9'; t++)
            ;
        for (; *t >= '0' && *t <= '9'; t++)
            ;
        for (; *t < '0' || *t > '9'; t++)
            ;
        for (; *t >= '0' && *t <= '9'; t++)
            ;

        // Now, read the the neighbors. Each one is an index to another vertex.
        for (j = 1; j <= A[i][0]; j++) {
            if (sscanf(t, "%ld", &A[i][j]) != 1) {
                (void) printf("Error while reading neighbour %ld of %ld of %s\n", j, i, name);
                exit(15);
            }

            // Skip over the number that was just parsed.
            for (; *t < '0' || *t > '9'; t++)
                 ;
            for (; *t >= '0' && *t <= '9'; t++)
                 ;
        }   /* j */
    }   /* i */

    /* Reading coordinates */
    // Read the "coordinates" for the configuration. If the parameter `C` is not
    // NULL, write the coordinates into `C`.
    //
    // `ReadConf()` is only ever called once (in `main()`) and it is passed NULL
    // for `C`. Thus, this section could probably eventually be eliminated.
    if (C != NULL) {
        C[0] = n;
    }

    // Read one coordinate for each vertex.
    for (i = 1; i <= n;) {
        (void) fgets(S, sizeof(S), F);
        if (C == NULL) {
            // Since `C` is NULL, discard the coordinates by writing them all into
            // the same variable, which previously was a loop index.
            // (but the coordinates are still read? Why?)
            k = sscanf(S, "%ld%ld%ld%ld%ld%ld%ld%ld", &j, &j, &j, &j, &j, &j, &j, &j);
        } else {
            // Read (up to) eight coordinates at a time.
            // (`k` is the number of coordinates successfully read.)
            k = sscanf(S, "%ld%ld%ld%ld%ld%ld%ld%ld", C + i, C + i + 1, C + i + 2, C + i + 3, C + i + 4, C + i + 5, C + i + 6, C + i + 7);
        }

        if (k == 0) {
            (void) printf("Error while reading coordinates of %s\n", name);
            exit(17);
        }

        i += k;
    }   /* for i */

    // Ensure that the configuration is followed by a blank line.
    (void) fgets(S, sizeof(S), F);
    for (t = S; *t == ' ' || *t == '\t'; t++)
        ;
    if (*t != '\n' && *t != '\0') {
        (void) printf("No blank line following configuration %s\n", name);
        exit(18);
    }

    // Verify various well-formedness conditions for the configuration.
    // TODO: Identify what those well-formedness conditions are.

    // Condition 1:
    // 2 <= r; r < n.
    if (r < 2 || n <= r) {
      ReadErr(1, name);
    }

    // Condition 2:
    // For each vertex i:
    // If i <= r:
    // degree(i) >= 3 && degree(i) <= n - 1
    // Else (i > r && i <= n):
    // degree(i) >= 5 && degree(i) <= n - 1
    //
    // (Equivalently,)
    // For each vertex i:
    // degree(i) <= n - 1.
    // If i is in the outer ring (i.e., i <= r):
    // degree(i) >= 3
    // Otherwise:
    // degree(i) >= 5
    for (i = 1; i <= r; i++) {
        if (A[i][0] < 3 || A[i][0] >= n) {
            ReadErr(2, name);
        }
    }
    for (i = r + 1; i <= n; i++) {
        if (A[i][0] < 5 || A[i][0] >= n) {
            ReadErr(2, name);
        }
    }

    // Condition 3:
    // Every entry in an adjacency list is in bounds. (1 <= vert <= n)
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= A[i][0]; j++) {
            if (A[i][j] < 1 || A[i][j] > n) {
                ReadErr(3, name);
            }
        }
    }

    // Condition 4:
    // For vertices in the outer ring (i <= r),
    // * The first entry of the adjacency list is the next ring vertex.
    // * The last entry of the adjacency list is the previous ring vertex.
    // * The other entries of the adjacency list are not ring vertices.
    for (i = 1; i <= r; i++) {
        if (A[i][1] != (i == r ? 1 : i + 1)) {
            ReadErr(4, name);
        }

        if (A[i][A[i][0]] != (i == 1 ? r : i - 1)) {
            ReadErr(4, name);
        }

        for (j = 2; j < A[i][0]; j++) {
            if (A[i][j] <= r || A[i][j] > n) {
                ReadErr(4, name);
            }
        }
    }

    // Condition 5:
    // The sum of the degrees of all the vertices must be 6*n - 6 - 2*r.
    for (i = 1, k = 0; i <= n; i++) {
        k += A[i][0];
    }

    if (k != 6 * (n - 1) - 2 * r) {
        ReadErr(5, name);
    }

    // Condition 6:
    // For vertices not in the outer ring (i > r):
    // There are up to two edges j such that a[i][j] is not a ring vertex and
    // a[i][j + 1] is a ring vertex. If there are two such edges, then in both
    // cases a[i][j + 2] is not a ring vertex.
    for (i = r + 1; i <= n; i++) {
        k = 0;
        d = A[i][0];
        for (j = 1; j <= d; j++) {
            if (A[i][j] > r && A[i][j < d ? j + 1 : 1] <= r) {
                k++;
                if (A[i][j < d - 1 ? j + 2 : j + 2 - d] <= r) {
                    k++;
                }
            }
        }

        if (k > 2) {
            ReadErr(6, name);
        }
    }

    // Condition 7:
    // For each vertex i:
    // There is a vertex p such that:
    // k = A[i][j]
    // A[i][j + 1] = A[k][p]
    // A[k][p + 1] = i
    // (I'm not entirely sure what this means. A diagram would be useful.)
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= A[i][0]; j++) {
            if (j == A[i][0]) {
                if (i <= r) {
                    continue;
                }

                a = A[i][1];
            } else {
                a = A[i][j + 1];
            }

            k = A[i][j];
            for (p = 1; p < A[k][0]; p++) {
                if (a == A[k][p] && i == A[k][p + 1]) {
                    break;
                }
            }

            if (p == A[k][0] && (a != A[k][p] || i != A[k][1])) {
                ReadErr(7, name);
            }
        }
    }

    return (long) 0;
}

void
ReadErr(int n, char name[])
{
    (void) printf("Error %d while reading configuration %s\n", n, name);
    exit(57);
}

/* vim: set et ts=4 sw=4 sts=4: */
