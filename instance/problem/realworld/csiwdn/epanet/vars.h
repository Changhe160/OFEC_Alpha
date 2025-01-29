/*
************************************************************************
            Global Variables for EPANET Program                            
                                                                    
VERSION:    2.00                                               
DATE:       5/8/00
            6/24/02
            2/14/08    (2.00.12)
AUTHOR:     L. Rossman                                         
            US EPA - NRMRL
                                                                     
************************************************************************
*/
#ifndef EPANET_VARS_H
#define EPANET_VARS_H

#include "hash.h"
#include "types.h"
#include "mempool.h"
#include <cstdio>

namespace epanet {
	class Vars {
	protected:
		 FILE     *InFile = nullptr,               /* Input file pointer           */
			*OutFile = nullptr,              /* Output file pointer          */
			*RptFile = nullptr,              /* Report file pointer          */
			*HydFile = nullptr,              /* Hydraulics file pointer      */
			*TmpOutFile = nullptr;           /* Temporary file handle        */
		 long     HydOffset,             /* Hydraulics file byte offset  */
			OutOffset1,            /* 1st output file byte offset  */
			OutOffset2;            /* 2nd output file byte offset  */
		 char     Msg[MAXMSG + 1],         /* Text of output message       */
			InpFname[MAXFNAME + 1],  /* Input file name              */
			Rpt1Fname[MAXFNAME + 1], /* Primary report file name     */
			Rpt2Fname[MAXFNAME + 1], /* Secondary report file name   */
			HydFname[MAXFNAME + 1],  /* Hydraulics file name         */
			OutFname[MAXFNAME + 1],  /* Binary output file name      */
			MapFname[MAXFNAME + 1],  /* Map file name                */
			TmpFname[MAXFNAME + 1],  /* Temporary file name          */      //(2.00.12 - LR)
			TmpDir[MAXFNAME + 1],    /* Temporary directory name     */      //(2.00.12 - LR)
			Title[MAXTITLE][MAXMSG + 1], /* Problem title            */
			ChemName[MAXID + 1],     /* Name of chemical             */
			ChemUnits[MAXID + 1],    /* Units of chemical            */
			DefPatID[MAXID + 1],     /* Default demand pattern ID    */

	/*** Updated 6/24/02 ***/
			Atime[13],             /* Clock time (hrs:min:sec)     */

			Outflag,               /* Output file flag             */      //(2.00.12 - LR)
			Hydflag,               /* Hydraulics flag              */
			Qualflag,              /* Water quality flag           */
			Reactflag,             /* Reaction indicator           */      //(2.00.12 - LR)
			Unitsflag,             /* Unit system flag             */
			Flowflag,              /* Flow units flag              */
			Pressflag,             /* Pressure units flag          */
			Formflag,              /* Hydraulic formula flag       */
			Rptflag,               /* Report flag                  */
			Summaryflag,           /* Report summary flag          */
			Messageflag,           /* Error/warning message flag   */
			Statflag,              /* Status report flag           */
			Energyflag,            /* Energy report flag           */
			Nodeflag,              /* Node report flag             */
			Linkflag,              /* Link report flag             */
			Tstatflag,             /* Time statistics flag         */
			Warnflag,              /* Warning flag                 */
			Openflag,              /* Input processed flag         */
			OpenHflag,             /* Hydraul. system opened flag  */
			SaveHflag,             /* Hydraul. results saved flag  */
			OpenQflag,             /* Quality system opened flag   */
			SaveQflag,             /* Quality results saved flag   */
			Saveflag;              /* General purpose save flag    */
		 int      MaxNodes,              /* Node count from input file   */
			MaxLinks,              /* Link count from input file   */
			MaxJuncs,              /* Junction count               */
			MaxPipes,              /* Pipe count                   */
			MaxTanks,              /* Tank count                   */
			MaxPumps,              /* Pump count                   */
			MaxValves,             /* Valve count                  */
			MaxControls,           /* Control count                */
			MaxRules,              /* Rule count                   */
			MaxPats,               /* Pattern count                */
			MaxCurves,             /* Curve count                  */
			Nnodes,                /* Number of network nodes      */
			Ntanks,                /* Number of tanks              */
			Njuncs,                /* Number of junction nodes     */
			Nlinks,                /* Number of network links      */
			Npipes,                /* Number of pipes              */
			Npumps,                /* Number of pumps              */
			Nvalves,               /* Number of valves             */
			Ncontrols,             /* Number of simple controls    */
			Nrules,                /* Number of control rules      */
			Npats,                 /* Number of time patterns      */
			Ncurves,               /* Number of data curves        */
			Nperiods,              /* Number of reporting periods  */
			Ncoeffs,               /* Number of non-0 matrix coeffs*/
			DefPat,                /* Default demand pattern       */
			Epat,                  /* Energy cost time pattern     */
			MaxIter,               /* Max. hydraulic trials        */
			ExtraIter,             /* Extra hydraulic trials       */
			TraceNode,             /* Source node for flow tracing */
			PageSize,              /* Lines/page in output report  */
			CheckFreq,             /* Hydraulics solver parameter  */
			MaxCheck;              /* Hydraulics solver parameter  */
		 double   Ucf[MAXVAR],           /* Unit conversion factors      */
			Ctol,                  /* Water quality tolerance      */
			Htol,                  /* Hydraulic head tolerance     */
			Qtol,                  /* Flow rate tolerance          */
			RQtol,                 /* Flow resistance tolerance    */
			Hexp,                  /* Exponent in headloss formula */
			Qexp,                  /* Exponent in orifice formula  */
			Dmult,                 /* Demand multiplier            */
			Hacc,                  /* Hydraulics solution accuracy */
			DampLimit,             /* Solution damping threshold   */      //(2.00.12 - LR)
			BulkOrder,             /* Bulk flow reaction order     */
			WallOrder,             /* Pipe wall reaction order     */
			TankOrder,             /* Tank reaction order          */
			Kbulk,                 /* Global bulk reaction coeff.  */
			Kwall,                 /* Global wall reaction coeff.  */
			Climit,                /* Limiting potential quality   */
			Rfactor,               /* Roughness-reaction factor    */
			Diffus,                /* Diffusivity (sq ft/sec)      */
			Viscos,                /* Kin. viscosity (sq ft/sec)   */
			SpGrav,                /* Specific gravity             */
			Ecost,                 /* Base energy cost per kwh     */
			Dcost,                 /* Energy demand charge/kw/day  */
			Epump,                 /* Global pump efficiency       */
			Emax,                  /* Peak energy usage            */
			Dsystem,               /* Total system demand          */
			Wbulk,                 /* Avg. bulk reaction rate      */
			Wwall,                 /* Avg. wall reaction rate      */
			Wtank,                 /* Avg. tank reaction rate      */
			Wsource;               /* Avg. mass inflow             */
		 long     Tstart,                /* Starting time of day (sec)   */
			Hstep,                 /* Nominal hyd. time Step (sec) */
			Qstep,                 /* Quality time Step (sec)      */
			Pstep,                 /* Time pattern time Step (sec) */
			Pstart,                /* Starting pattern time (sec)  */
			Rstep,                 /* Reporting time Step (sec)    */
			Rstart,                /* Time when reporting starts   */
			Rtime,                 /* Next reporting time          */
			Htime,                 /* Current hyd. time (sec)      */
			Qtime,                 /* Current quality time (sec)   */
			Hydstep,               /* Actual hydraulic time Step   */
			Rulestep,              /* Rule evaluation time Step    */
			Dur;                   /* Duration of simulation (sec) */
		 SField   Field[MAXVAR];         /* Output reporting fields      */

		/* Array pointers not allocated and freed in same routine */
		 char     *S = nullptr,                    /* Link status                  */
			*OldStat = nullptr;              /* Previous link/tank status    */
		 double   *D = nullptr,                    /* Node actual demand           */
			*C = nullptr,                    /* Node actual quality          */
			*E = nullptr,                    /* Emitter flows                */
			*K = nullptr,                    /* Link settings                */
			*Q = nullptr,                    /* Link flows                   */
			*R = nullptr,                    /* Pipe reaction rate           */
			*X = nullptr;                    /* General purpose array        */
		 double   *H = nullptr;                    /* Node heads                   */
		 STmplist *Patlist = nullptr;              /* Temporary time pattern list  */
		 STmplist *Curvelist = nullptr;            /* Temporary list of curves     */
		 Spattern *Pattern = nullptr;              /* Time patterns                */
		 Scurve   *Curve = nullptr;                /* Curve data                   */
		 Snode    *Node = nullptr;                 /* Node data                    */
		 Slink    *Link = nullptr;                 /* Link data                    */
		 Stank    *Tank = nullptr;                 /* Tank data                    */
		 Spump    *Pump = nullptr;                 /* Pump data                    */
		 Svalve   *Valve = nullptr;                /* Valve data                   */
		 Scontrol *Control = nullptr;              /* Control data                 */
		 HTtable  *Nht = nullptr, *Lht = nullptr;            /* Hash tables for ID labels    */
		 Padjlist *Adjlist = nullptr;              /* Node adjacency lists         */

		/*
		** NOTE: Hydraulic analysis of the pipe network at a given point in time
		**       is done by repeatedly solving a linearized version of the
		**       equations for conservation of flow & energy:
		**
		**           A*H = F
		**
		**       where H = vector of heads (unknowns) at each node,
		**             F = vector of right-hand side coeffs.
		**             A = square matrix of coeffs.
		**       and both A and F are updated at each iteration until there is
		**       negligible change in pipe flows.
		**
		**       Each row (or column) of A corresponds to a junction in the pipe
		**       network. Each link (pipe, pump or valve) in the network has a
		**       non-zero entry in the row-column of A that corresponds to its
		**       end points. This results in A being symmetric and very sparse.
		**       The following arrays are used to efficiently manage this sparsity:
		*/

		 double   *Aii = nullptr,        /* Diagonal coeffs. of A               */
			*Aij = nullptr,        /* Non-zero, off-diagonal coeffs. of A */
			*F = nullptr;          /* Right hand side coeffs.             */
		 double   *P = nullptr,          /* Inverse headloss derivatives        */
			*Y = nullptr;          /* Flow correction factors             */
		 int      *Order = nullptr,      /* Node-to-row of A                    */
			*Row = nullptr,        /* Row-to-node of A                    */
			*Ndx = nullptr;        /* Index of link's coeff. in Aij       */
	/*
	** The following arrays store the positions of the non-zero coeffs.
	** of the lower triangular portion of A whose values are stored in Aij:
	*/
		 int      *XLNZ = nullptr,       /* Start position of each column in NZSUB  */
			*NZSUB = nullptr,      /* Row index of each coeff. in each column */
			*LNZ = nullptr;        /* Position of each coeff. in Aij array    */

		 /*input2*/
		 int    Ntokens,           /* Number of tokens in input line    */
			 Ntitle;            /* Number of title lines             */
		 char   *Tok[MAXTOKS];     /* Array of token strings            */

								   /* Used in INPUT3.C: */
		 STmplist  *PrevPat = nullptr;       /* Pointer to pattern list element   */
		 STmplist  *PrevCurve = nullptr;     /* Pointer to curve list element     */

		 /*rules*/
		 struct  aRule *Rule = nullptr;        /* Array of rules */
		 struct  ActItem *ActList = nullptr;   /* Linked list of action items */
		 int     RuleState;          /* State of rule interpreter */
		 long    Time1;              /* Start of rule evaluation time interval (sec) */
		 struct  Premise *Plast = nullptr;     /* Previous premise clause */

		 /*report*/
		 long      LineNum;        /* Current line number     */
		 long      PageNum;        /* Current page number     */
		 char      DateStamp[26];  /* Current date & time     */
		 char      Fprinterr;      /* File write error flag   */

		 /*hydraul*/
		 int Haltflag;
		        /* Relaxation factor used for updating flow changes */                         //(2.00.11 - LR)
		 double RelaxFactor;

		 /*smatrix*/
		 int      *Degree = nullptr;     /* Number of links adjacent to each node  */

		 /*quality*/
		 Pseg      FreeSeg;              /* Pointer to unused segment               */
		 Pseg      *FirstSeg = nullptr,            /* First (downstream) segment in each pipe */
			 *LastSeg = nullptr;             /* Last (upstream) segment in each pipe    */
		 char      *FlowDir = nullptr;             /* Flow direction for each pipe            */
		 double    *VolIn = nullptr;               /* Total volume inflow to node             */
		 double    *MassIn = nullptr;              /* Total mass inflow to node               */
		 double    Sc;                   /* Schmidt Number                          */
		 double    Bucf;                 /* Bulk reaction units conversion factor   */
		 double    Tucf;                 /* Tank reaction units conversion factor   */

										 /*** Moved to vars.h ***/                                                      //(2.00.12 - LR)
																														//char      Reactflag;            /* Reaction indicator                      */

		 char      OutOfMemory;          /* Out of memory indicator                 */
		 alloc_handle_t *SegPool = nullptr; // Memory pool for water quality segments   //(2.00.11 - LR)   static
	};
}
#endif //!EPANET_VARS_H