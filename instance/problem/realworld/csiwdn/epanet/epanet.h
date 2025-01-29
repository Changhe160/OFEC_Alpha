#ifndef OFEC_EPANET_H
#define OFEC_EPANET_H

#include "vars.h"

// --- Define the EPANET toolkit constants

#define EN_ELEVATION    0    /* Node parameters */
#define EN_BASEDEMAND   1
#define EN_PATTERN      2
#define EN_EMITTER      3
#define EN_INITQUAL     4
#define EN_SOURCEQUAL   5
#define EN_SOURCEPAT    6
#define EN_SOURCETYPE   7
#define EN_TANKLEVEL    8
#define EN_DEMAND       9
#define EN_HEAD         10
#define EN_PRESSURE     11
#define EN_QUALITY      12
#define EN_SOURCEMASS   13
#define EN_INITVOLUME   14
#define EN_MIXMODEL     15
#define EN_MIXZONEVOL   16

#define EN_TANKDIAM     17
#define EN_MINVOLUME    18
#define EN_VOLCURVE     19
#define EN_MINLEVEL     20
#define EN_MAXLEVEL     21
#define EN_MIXFRACTION  22
#define EN_TANK_KBULK   23

#define EN_DIAMETER     0    /* Link parameters */
#define EN_LENGTH       1
#define EN_ROUGHNESS    2
#define EN_MINORLOSS    3
#define EN_INITSTATUS   4
#define EN_INITSETTING  5
#define EN_KBULK        6
#define EN_KWALL        7
#define EN_FLOW         8
#define EN_VELOCITY     9
#define EN_HEADLOSS     10
#define EN_STATUS       11
#define EN_SETTING      12
#define EN_ENERGY       13

#define EN_DURATION     0    /* Time parameters */
#define EN_HYDSTEP      1
#define EN_QUALSTEP     2
#define EN_PATTERNSTEP  3
#define EN_PATTERNSTART 4
#define EN_REPORTSTEP   5
#define EN_REPORTSTART  6
#define EN_RULESTEP     7
#define EN_STATISTIC    8
#define EN_PERIODS      9

#define EN_NODECOUNT    0    /* Component counts */
#define EN_TANKCOUNT    1
#define EN_LINKCOUNT    2
#define EN_PATCOUNT     3
#define EN_CURVECOUNT   4
#define EN_CONTROLCOUNT 5

#define EN_JUNCTION     0    /* Node types */
#define EN_RESERVOIR    1
#define EN_TANK         2

#define EN_CVPIPE       0    /* Link types */
#define EN_PIPE         1
#define EN_PUMP         2
#define EN_PRV          3
#define EN_PSV          4
#define EN_PBV          5
#define EN_FCV          6
#define EN_TCV          7
#define EN_GPV          8

#define EN_NONE         0    /* Quality analysis types */
#define EN_CHEM         1
#define EN_AGE          2
#define EN_TRACE        3

#define EN_CONCEN       0    /* Source quality types */
#define EN_MASS         1
#define EN_SETPOINT     2
#define EN_FLOWPACED    3

#define EN_CFS          0    /* Flow units types */
#define EN_GPM          1
#define EN_MGD          2
#define EN_IMGD         3
#define EN_AFD          4
#define EN_LPS          5
#define EN_LPM          6
#define EN_MLD          7
#define EN_CMH          8
#define EN_CMD          9

#define EN_TRIALS       0   /* Misc. options */
#define EN_ACCURACY     1
#define EN_TOLERANCE    2
#define EN_EMITEXPON    3
#define EN_DEMANDMULT   4

#define EN_LOWLEVEL     0   /* Control types */
#define EN_HILEVEL      1
#define EN_TIMER        2
#define EN_TIMEOFDAY    3

#define EN_AVERAGE      1   /* Time statistic types.    */
#define EN_MINIMUM      2 
#define EN_MAXIMUM      3
#define EN_RANGE        4

#define EN_MIX1         0   /* Tank mixing models */
#define EN_MIX2         1
#define EN_FIFO         2
#define EN_LIFO         3

#define EN_NOSAVE       0   /* Save-results-to-file flag */
#define EN_SAVE         1
#define EN_INITFLOW     10  /* Re-initialize flow flag   */

namespace epanet {
	char *gStrtok_r(char *s, const char *delim, char **save_ptr);

	typedef  float	REAL4;                                                   //(2.00.11 - LR)
	typedef  int    INT4;                                                    //(2.00.12 - LR)
	typedef  REAL4	*Pfloat;

	class EPANET : public Vars , public MemPool {
	public:
		int	   ENepanet(char *, char *, char *, void(*) (char *));
		int    ENopen(const char *, const char *, const char *);
		int    ENsaveinpfile(char *);
		int    ENclose(void);

		int    ENsolveH(void);
		int    ENsaveH(void);
		int    ENopenH(void);
		int    ENinitH(int);
		int    ENrunH(long *);
		int    ENnextH(long *);
		int    ENcloseH(void);
		int    ENsavehydfile(char *);
		int    ENusehydfile(char *);

		int    ENsolveQ(void);
		int    ENopenQ(void);
		int    ENinitQ(int);
		int    ENrunQ(long *);
		int    ENnextQ(long *);
		int    ENstepQ(long *);
		int    ENcloseQ(void);

		int    ENwriteline(char *);
		int    ENreport(void);
		int    ENresetreport(void);
		int    ENsetreport(char *);

		int    ENgetcontrol(int, int *, int *, float *, int *, float *);
		int    ENgetcount(int, int *);
		int    ENgetoption(int, float *);
		int    ENgettimeparam(int, long *);
		int    ENgetflowunits(int *);
		int    ENgetpatternindex(char *, int *);
		int    ENgetpatternid(int, char *);
		int    ENgetpatternlen(int, int *);
		int    ENgetpatternvalue(int, int, float *);
		int    ENgetqualtype(int *, int *);
		int    ENgeterror(int, char *, int);

		int    ENgetnodeindex(char *, int *);
		int    ENgetnodeid(int, char *);
		int    ENgetnodetype(int, int *);
		int    ENgetnodevalue(int, int, float *);

		int    ENgetlinkindex(char *, int *);
		int    ENgetlinkid(int, char *);
		int    ENgetlinktype(int, int *);
		int    ENgetlinknodes(int, int *, int *);
		int    ENgetlinkvalue(int, int, float *);

		int    ENgetversion(int *);

		int    ENsetcontrol(int, int, int, float, int, float);
		int    ENsetnodevalue(int, int, float);
		int    ENsetlinkvalue(int, int, float);
		int    ENaddpattern(char *);
		int    ENsetpattern(int, float *, int);
		int    ENsetpatternvalue(int, int, float);
		int    ENsettimeparam(int, long);
		int    ENsetoption(int, float);
		int    ENsetstatusreport(int);
		int    ENsetqualtype(int,const char *,const char *, const char *);

	private:
		void    initpointers(void);               /* Initializes pointers       */
		int     allocdata(void);                  /* Allocates memory           */
		void    freeTmplist(STmplist *);          /* Frees items in linked list */
		void    freeFloatlist(SFloatlist *);      /* Frees list of floats       */
		void    freedata(void);                   /* Frees allocated memory     */
		int     openfiles(const char *, const char *,const char *);  /* Opens input & report files */
		int     openhydfile(void);                /* Opens hydraulics file      */
		int     openoutfile(void);                /* Opens binary output file   */
		int     strcomp(const char *, const char *);          /* Compares two strings       */
		char *getTmpName(char *fname);          /* Gets temporary file name   */     //(2.00.12 - LR)
		double  interp(int, double *,             /* Interpolates a data curve  */
			double *, double);
		int     findnode(char *);                 /* Finds node's index from ID */
		int     findlink(char *);                 /* Finds link's index from ID */
		char *geterrmsg(int);                   /* Gets text of error message */
		void    errmsg(int);                      /* Reports program error      */
		void    writecon(const char *);                 /* Writes text to console     */
		void    writewin(const char *);                 /* Passes text to calling app */

												  /* ------- INPUT1.C --------------------*/
		int     getdata(void);                    /* Gets network data          */
		void    setdefaults(void);                /* Sets default values        */
		void    initreport(void);                 /* Initializes report options */
		void    adjustdata(void);                 /* Adjusts input data         */
		int     inittanks(void);                  /* Initializes tank levels    */
		void    initunits(void);                  /* Determines reporting units */
		void    convertunits(void);               /* Converts data to std. units*/

												  /* -------- INPUT2.C -------------------*/
		int     netsize(void);                    /* Determines network size    */
		int     readdata(void);                   /* Reads in network data      */
		int     newline(int, char *);             /* Processes new line of data */
		int     addnodeID(int, char *);           /* Adds node ID to data base  */
		int     addlinkID(int, char *);           /* Adds link ID to data base  */
		int     addpattern(const char *);               /* Adds pattern to data base  */
		int     addcurve(char *);                 /* Adds curve to data base    */
		STmplist *findID(const char *, STmplist *);     /* Locates ID on linked list  */
		int     unlinked(void);                   /* Checks for unlinked nodes  */
		int     getpumpparams(void);              /* Computes pump curve coeffs.*/
		int     getpatterns(void);                /* Gets pattern data from list*/
		int     getcurves(void);                  /* Gets curve data from list  */
		int     findmatch(const char *,const char *[]);       /* Finds keyword in line      */
		int     match(const char *, const char *);            /* Checks for word match      */
		int     gettokens(char *);                /* Tokenizes input line       */
		int     getfloat(char *, double *);       /* Converts string to double   */
		double  hour(char *, const char *);             /* Converts time to hours     */
		int     setreport(char *);                /* Processes reporting command*/
		void    inperrmsg(int, int, char *);        /* Input error message        */

													/* ---------- INPUT3.C -----------------*/
		int     juncdata(void);                   /* Processes junction data    */
		int     tankdata(void);                   /* Processes tank data        */
		int     pipedata(void);                   /* Processes pipe data        */
		int     pumpdata(void);                   /* Processes pump data        */
		int     valvedata(void);                  /* Processes valve data       */
		int     patterndata(void);                /* Processes pattern data     */
		int     curvedata(void);                  /* Processes curve data       */
		int     demanddata(void);                 /* Processes demand data      */
		int     controldata(void);                /* Processes simple controls  */
		int     energydata(void);                 /* Processes energy data      */
		int     sourcedata(void);                 /* Processes source data      */
		int     emitterdata(void);                /* Processes emitter data     */
		int     qualdata(void);                   /* Processes quality data     */
		int     reactdata(void);                  /* Processes reaction data    */
		int     mixingdata(void);                 /* Processes tank mixing data */
		int     statusdata(void);                 /* Processes link status data */
		int     reportdata(void);                 /* Processes report options   */
		int     timedata(void);                   /* Processes time options     */
		int     optiondata(void);                 /* Processes analysis options */
		int     optionchoice(int);                /* Processes option choices   */
		int     optionvalue(int);                 /* Processes option values    */
		int     getpumpcurve(int);                /* Constructs a pump curve    */
		int     powercurve(double, double, double,/* Coeffs. of power pump curve*/
			double, double, double *,
			double *, double *);
		int     valvecheck(int, int, int);        /* Checks valve placement     */
		void    changestatus(int, char, double);  /* Changes status of a link   */

												  /* -------------- RULES.C --------------*/
		void    initrules(void);                  /* Initializes rule base      */
		void    addrule(char *);                  /* Adds rule to rule base     */
		int     allocrules(void);                 /* Allocates memory for rule  */
		int     ruledata(void);                   /* Processes rule input data  */
		int     checkrules(long);                 /* Checks all rules           */
		void    freerules(void);                  /* Frees rule base memory     */

		void    newrule(void);
		int     newpremise(int);
		int     newaction(void);
		int     newpriority(void);
		int     evalpremises(int);
		void    updateactlist(int, Action *);
		int     checkaction(int, Action *);
		int     checkpremise(Premise *);
		int     checktime(Premise *);
		int     checkstatus(Premise *);
		int     checkvalue(Premise *);
		int     takeactions(void);
		void    clearactlist(void);
		void    clearrules(void);
		void    ruleerrmsg(int);

		/* ------------- REPORT.C --------------*/
		int     writereport(void);                /* Writes formatted report    */
		void    writelogo(void);                  /* Writes program logo        */
		void    writesummary(void);               /* Writes network summary     */
		void    writehydstat(int, double);          /* Writes hydraulic status    */
		void    writeenergy(void);                /* Writes energy usage        */
		int     writeresults(void);               /* Writes node/link results   */
		void    writeheader(int, int);             /* Writes heading on report   */
		void    writeline(const char *);                /* Writes line to report file */
		void    writerelerr(int, double);          /* Writes convergence error   */
		void    writestatchange(int, char, char);   /* Writes link status change  */
		void    writecontrolaction(int, int);     /* Writes control action taken*/
		void    writeruleaction(int, char *);     /* Writes rule action taken   */
		int     writehydwarn(int, double);          /* Writes hydraulic warnings  */
		void    writehyderr(int);                 /* Writes hydraulic error msg.*/
		int     disconnected(void);               /* Checks for disconnections  */
		void    marknodes(int, int *, char *);    /* Identifies connected nodes */
		void    getclosedlink(int, char *);       /* Finds a disconnecting link */
		void    writelimits(int, int);             /* Writes reporting limits    */
		int     checklimits(double *, int, int);     /* Checks variable limits     */
		void    writetime(const char *);                /* Writes current clock time  */
		char *clocktime(char *, long);         /* Converts time to hrs:min   */
		char *fillstr(char *, char, int);      /* Fills string with character*/
		int     getnodetype(int);                 /* Determines node type       */

		void      writenodetable(Pfloat *);
		void      writelinktable(Pfloat *);

		/* --------- HYDRAUL.C -----------------*/
		int     openhyd(void);                    /* Opens hydraulics solver    */

												  /*** Updated 3/1/01 ***/
		void    inithyd(int);                     /* Re-sets initial conditions */

		int     runhyd(long *);                   /* Solves 1-period hydraulics */
		int     nexthyd(long *);                  /* Moves to next time period  */
		void    closehyd(void);                   /* Closes hydraulics solver   */
		int     allocmatrix(void);                /* Allocates matrix coeffs.   */
		void    freematrix(void);                 /* Frees matrix coeffs.       */
		void    initlinkflow(int, char, double);  /* Initializes link flow      */
		void    setlinkflow(int, double);         /* Sets link flow via headloss*/
		void    setlinkstatus(int, char, char *,  /* Sets link status           */
			double *);
		void    setlinksetting(int, double,       /* Sets pump/valve setting    */
			char *, double *);
		void    resistance(int);                  /* Computes resistance coeff. */
		void    demands(void);                    /* Computes current demands   */
		int     controls(void);                   /* Controls link settings     */
		long    timestep(void);                   /* Computes new time Step     */
		void    tanktimestep(long *);             /* Time till tanks fill/drain */
		void    controltimestep(long *);          /* Time till control action   */
		void    ruletimestep(long *);             /* Time till rule action      */
		void    addenergy(long);                  /* Accumulates energy usage   */
		void    getenergy(int, double *, double *); /* Computes link energy use   */
		void    tanklevels(long);                 /* Computes new tank levels   */
		double  tankvolume(int, double);           /* Finds tank vol. from grade */
		double  tankgrade(int, double);            /* Finds tank grade from vol. */
		int     netsolve(int *, double *);         /* Solves network equations   */
		int     badvalve(int);                    /* Checks for bad valve       */
		int     valvestatus(void);                /* Updates valve status       */
		int     linkstatus(void);                 /* Updates link status        */
		char    cvstatus(char, double, double);     /* Updates CV status          */
		char    pumpstatus(int, double);           /* Updates pump status        */
		char    prvstatus(int, char, double,        /* Updates PRV status         */
			double, double);
		char    psvstatus(int, char, double,        /* Updates PSV status         */
			double, double);
		char    fcvstatus(int, char, double,        /* Updates FCV status         */
			double);
		void    tankstatus(int, int, int);          /* Checks if tank full/empty  */
		int     pswitch(void);                    /* Pressure switch controls   */
		double  newflows(void);                   /* Updates link flows         */
		void    newcoeffs(void);                  /* Computes matrix coeffs.    */
		void    linkcoeffs(void);                 /* Computes link coeffs.      */
		void    nodecoeffs(void);                 /* Computes node coeffs.      */
		void    valvecoeffs(void);                /* Computes valve coeffs.     */
		void    pipecoeff(int);                   /* Computes pipe coeff.       */
		double  DWcoeff(int, double *);           /* Computes D-W coeff.        */
		void    pumpcoeff(int);                   /* Computes pump coeff.       */

												  /*** Updated 10/25/00 ***/
												  /*** Updated 12/29/00 ***/
		void    curvecoeff(int, double, double *,   /* Computes curve coeffs.     */
			double *);

		void    gpvcoeff(int);                    /* Computes GPV coeff.        */
		void    pbvcoeff(int);                    /* Computes PBV coeff.        */
		void    tcvcoeff(int);                    /* Computes TCV coeff.        */
		void    prvcoeff(int, int, int);            /* Computes PRV coeff.        */
		void    psvcoeff(int, int, int);            /* Computes PSV coeff.        */
		void    fcvcoeff(int, int, int);            /* Computes FCV coeff.        */
		void    emittercoeffs(void);              /* Computes emitter coeffs.   */
		double  emitflowchange(int);              /* Computes new emitter flow  */

		void valvecoeff(int k);

		/* ----------- SMATRIX.C ---------------*/
		int     createsparse(void);               /* Creates sparse matrix      */
		int     allocsparse(void);                /* Allocates matrix memory    */
		void    freesparse(void);                 /* Frees matrix memory        */
		int     buildlists(int);                  /* Builds adjacency lists     */
		int     paralink(int, int, int);          /* Checks for parallel links  */
		void    xparalinks(void);                 /* Removes parallel links     */
		void    freelists(void);                  /* Frees adjacency lists      */
		void    countdegree(void);                /* Counts links at each node  */
		int     reordernodes(void);               /* Finds a node re-ordering   */
		int     mindegree(int, int);              /* Finds min. degree node     */
		int     growlist(int);                    /* Augments adjacency list    */
		int     newlink(Padjlist);                /* Adds fill-ins for a node   */
		int     linked(int, int);                 /* Checks if 2 nodes linked   */
		int     addlink(int, int, int);           /* Creates new fill-in        */
		int     storesparse(int);                 /* Stores sparse matrix       */
		int     ordersparse(int);                 /* Orders matrix storage      */
		void    transpose(int, int *, int *,        /* Transposes sparse matrix   */
			int *, int *, int *, int *, int *);
		int     linsolve(int, double *, double *, /* Solution of linear eqns.   */
			double *);               /* via Cholesky factorization */

									 /* ----------- QUALITY.C ---------------*/
		int     openqual(void);                   /* Opens WQ solver system     */
		void    initqual(void);                   /* Initializes WQ solver      */
		int     runqual(long *);                  /* Gets current WQ results    */
		int     nextqual(long *);                 /* Updates WQ by hyd.timestep */
		int     stepqual(long *);                 /* Updates WQ by WQ time Step */
		int     closequal(void);                  /* Closes WQ solver system    */
		int     gethyd(long *, long *);           /* Gets next hyd. results     */
		char    setReactflag(void);               /* Checks for reactive chem.  */
		void    transport(long);                  /* Transports mass in network */
		void    initsegs(void);                   /* Initializes WQ segments    */
		void    reorientsegs(void);               /* Re-orients WQ segments     */
		void    updatesegs(long);                 /* Updates quality in segments*/
		void    removesegs(int);                  /* Removes a WQ segment       */
		void    addseg(int, double, double);        /* Adds a WQ segment to pipe  */
		void    accumulate(long);                 /* Sums mass flow into node   */
		void    updatenodes(long);                /* Updates WQ at nodes        */
		void    sourceinput(long);                /* Computes source inputs     */
		void    release(long);                    /* Releases mass from nodes   */
		void    updatetanks(long);                /* Updates WQ in tanks        */
		void    updatesourcenodes(long);          /* Updates WQ at source nodes */
		void    tankmix1(int, long);              /* Complete mix tank model    */
		void    tankmix2(int, long);              /* 2-compartment tank model   */
		void    tankmix3(int, long);              /* FIFO tank model            */
		void    tankmix4(int, long);              /* LIFO tank model            */
		double  sourcequal(Psource);              /* Finds WQ input from source */
		double  avgqual(int);                     /* Finds avg. quality in pipe */
		void    ratecoeffs(void);                 /* Finds wall react. coeffs.  */
		double  piperate(int);                    /* Finds wall react. coeff.   */
		double  pipereact(int, double, double, long);/* Reacts water in a pipe     */
		double  tankreact(double, double, double,
			long);                  /* Reacts water in a tank     */
		double  bulkrate(double, double, double);   /* Finds bulk reaction rate   */
		double  wallrate(double, double, double, double);/* Finds wall reaction rate   */

		double getucf(double order);

		/* ------------ OUTPUT.C ---------------*/
		int     savenetdata(void);                /* Saves basic data to file   */
		int     savehyd(long *);                  /* Saves hydraulic solution   */
		int     savehydstep(long *);              /* Saves hydraulic timestep   */
		int     saveenergy(void);                 /* Saves energy usage         */
		int     readhyd(long *);                  /* Reads hydraulics from file */
		int     readhydstep(long *);              /* Reads time Step from file  */
		int     saveoutput(void);                 /* Saves results to file      */
		int     nodeoutput(int, REAL4 *, double); /* Saves node results to file */
		int     linkoutput(int, REAL4 *, double); /* Saves link results to file */
		int     savefinaloutput(void);            /* Finishes saving output     */
		int     savetimestat(REAL4 *, char);      /* Saves time stats to file   */
		int     savenetreacts(double, double,
			double, double);    /* Saves react. rates to file */
		int     saveepilog(void);                 /* Saves output file epilog   */

												  /* ------------ INPFILE.C --------------*/
		int     saveinpfile(char *);              /* Saves network to text file  */
		void    saveauxdata(FILE *f);

	};
}

#endif // !OFEC_EPANET_H
