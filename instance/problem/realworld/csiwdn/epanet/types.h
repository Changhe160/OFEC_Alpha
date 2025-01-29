/*------------------------------------------------------
   Global Data Structures
------------------------------------------------------*/

#ifndef EPANET_TYPES_H
#define EPANET_TYPES_H

#define MAXTITLE  3		/* Max. # title lines                     */
#define MAXID     31	/* Max. # characters in ID name           */      //(2.00.11 - LR)
#define MAXMSG    79	/* Max. # characters in message text      */
#define MAXLINE   255	/* Max. # characters read from input line */
#define MAXFNAME  259	/* Max. # characters in file name         */
#define MAXTOKS   40	/* Max. items per line of input           */
#define MAXVAR    21	/* Max. # types of network variables      */
#define	MAXERRS   10	/* Max. input errors reported             */
#define MAXCOUNT  10	/* Max. # of disconnected nodes listed    */

namespace epanet {
	struct IDstring    /* Holds component ID labels */
	{
		char ID[MAXID + 1];
	};

	struct  Floatlist  /* Element of list of floats */
	{
		double  value;
		struct  Floatlist *next;
	};
	typedef struct Floatlist SFloatlist;

	struct  Tmplist    /* Element of temp list for Pattern & Curve data */
	{
		int        i;
		char       ID[MAXID + 1];
		SFloatlist *x;
		SFloatlist *y;
		struct     Tmplist *next;
	};
	typedef struct Tmplist STmplist;

	typedef struct        /* TIME PATTERN OBJECT */
	{
		char   ID[MAXID + 1]; /* Pattern ID       */
		int    Length;      /* Pattern length   */
		double *F;          /* Pattern factors  */
	}  Spattern;

	typedef struct        /* CURVE OBJECT */
	{
		char   ID[MAXID + 1]; /* Curve ID         */
		int    Type;        /* Curve type       */
		int    Npts;        /* Number of points */
		double *X;          /* X-values         */
		double *Y;          /* Y-values         */
	}  Scurve;

	struct Sdemand            /* DEMAND CATEGORY OBJECT */
	{
		double Base;            /* Baseline demand  */
		int    Pat;             /* Pattern index    */
		struct Sdemand *next;   /* Next record      */
	};
	typedef struct Sdemand *Pdemand; /* Pointer to demand object */

	struct Ssource     /* WQ SOURCE OBJECT */
	{
		/*int   Node;*/     /* Node index of source     */
		double C0;       /* Base concentration/mass  */
		int    Pat;      /* Pattern index            */
		double Smass;    /* Actual mass flow rate    */
		char   Type;     /* SourceType (see below)   */
	};
	typedef struct Ssource *Psource; /* Pointer to WQ source object */

	typedef struct            /* NODE OBJECT */
	{
		char    ID[MAXID + 1];    /* Node ID          */
		double  El;             /* Elevation        */
		Pdemand D;              /* Demand pointer   */
		Psource S;              /* Source pointer   */
		double  C0;             /* Initial quality  */
		double  Ke;             /* Emitter coeff.   */
		char    Rpt;            /* Reporting flag   */
	}  Snode;

	typedef struct            /* LINK OBJECT */
	{
		char    ID[MAXID + 1];    /* Link ID           */
		int     N1;             /* Start node index  */
		int     N2;             /* End node index    */
		double  Diam;           /* Diameter          */
		double  Len;            /* Length            */
		double  Kc;             /* Roughness         */
		double  Km;             /* Minor loss coeff. */
		double  Kb;             /* Bulk react. coeff */
		double  Kw;             /* Wall react. coeff */
		double  R;              /* Flow resistance   */
		char    Type;           /* Link type         */
		char    Stat;           /* Initial status    */
		char    Rpt;            /* Reporting flag    */
	}  Slink;

	typedef struct     /* TANK OBJECT */
	{
		int    Node;     /* Node index of tank       */
		double A;        /* Tank area                */
		double Hmin;     /* Minimum water elev       */
		double Hmax;     /* Maximum water elev       */
		double H0;       /* Initial water elev       */
		double Vmin;     /* Minimum volume           */
		double Vmax;     /* Maximum volume           */
		double V0;       /* Initial volume           */
		double Kb;       /* Reaction coeff. (1/days) */
		double V;        /* Tank volume              */
		double C;        /* Concentration            */
		int    Pat;      /* Fixed grade time pattern */
		int    Vcurve;   /* Vol.- elev. curve index  */
		char   MixModel; /* Type of mixing model     */
						 /* (see MixType below)      */
		double V1max;    /* Mixing compartment size  */
	}  Stank;

	typedef struct     /* PUMP OBJECT */
	{
		int    Link;     /* Link index of pump          */
		int    Ptype;    /* Pump curve type             */
						 /* (see PumpType below)        */
		double Q0;       /* Initial flow                */
		double Qmax;     /* Maximum flow                */
		double Hmax;     /* Maximum head                */
		double H0;       /* Shutoff head                */
		double R;        /* Flow coeffic.               */
		double N;        /* Flow exponent               */
		int    Hcurve;   /* Head v. flow curve index    */
		int    Ecurve;   /* Effic. v. flow curve index  */
		int    Upat;     /* Utilization pattern index   */
		int    Epat;     /* Energy cost pattern index   */
		double Ecost;    /* Unit energy cost            */
		double Energy[6];  /* Energy usage statistics:  */
						  /* 0 = pump utilization      */
						  /* 1 = avg. efficiency       */
						  /* 2 = avg. kW/flow          */
						  /* 3 = avg. kwatts           */
						  /* 4 = peak kwatts           */
						  /* 5 = cost/day              */
	}  Spump;

	typedef struct     /* VALVE OBJECT */
	{
		int   Link;     /* Link index of valve */
	}  Svalve;

	typedef struct     /* CONTROL STATEMENT */
	{
		int    Link;     /* Link index         */
		int    Node;     /* Control node index */
		long   Time;     /* Control time       */
		double Grade;    /* Control grade      */
		double Setting;  /* New link setting   */
		char   Status;   /* New link status    */
		char   Type;     /* Control type       */
						/* (see ControlType below) */
	}  Scontrol;

	struct   Sadjlist         /* NODE ADJACENCY LIST ITEM */
	{
		int    node;            /* Index of connecting node */
		int    link;            /* Index of connecting link */
		struct Sadjlist *next;  /* Next item in list        */
	};
	/* Pointer to adjacency list item */
	typedef struct Sadjlist *Padjlist;

	struct  Sseg               /* PIPE SEGMENT record used */
	{                          /*   for WQ routing         */
		double  v;              /* Segment volume      */
		double  c;              /* Water quality value */
		struct  Sseg *prev;     /* Record for previous segment */
	};
	typedef struct Sseg *Pseg;    /* Pointer to pipe segment */

	typedef struct            /* FIELD OBJECT of report table */
	{
		char   Name[MAXID + 1];   /* Name of reported variable  */
		char   Units[MAXID + 1];  /* Units of reported variable */
		char   Enabled;         /* Enabled if in table        */
		int    Precision;       /* Number of decimal places   */
		double RptLim[2];       /* Lower/upper report limits  */
	} SField;

	/*rules*/

	struct      Premise         /* Rule Premise Clause */
	{
		int      logop;          /* Logical operator */
		int      object;         /* Node or link */
		int      index;          /* Object's index */
		int      variable;       /* Pressure, flow, etc. */
		int      relop;          /* Relational operator */
		int      status;         /* Variable's status */
		double    value;          /* Variable's value */
		struct   Premise *next;
	};

	struct     Action           /* Rule Action Clause */
	{
		int     link;            /* Link index */
		int     status;          /* Link's status */
		double   setting;         /* Link's setting */
		struct  Action *next;
	};

	struct      aRule           /* Control Rule Structure */
	{
		char     label[MAXID + 1];    /* Rule character label */
		double    priority;          /* Priority level */
		struct   Premise *Pchain;  /* Linked list of premises */
		struct   Action *Tchain;  /* Linked list of actions if true */
		struct   Action *Fchain;  /* Linked list of actions if false */
		struct   aRule *next;
	};

	struct      ActItem         /* Action list item */
	{
		int      ruleindex;        /* Index of rule action belongs to */
		struct   Action *action; /* An action structure */
		struct   ActItem *next;
	};

	/*
	----------------------------------------------
	   Global Enumeration Variables
	----------------------------------------------
	*/
	enum Hydtype                   /* Hydraulics solution option:         */
	{
		USE,           /*    use from previous run            */
		SAVE,          /*    save after current run           */
		SCRATCH
	};      /*    use temporary file               */

	enum QualType                  /* Water quality analysis option:      */
	{
		NONE,          /*    no quality analysis              */
		CHEM,          /*    analyze a chemical               */
		AGE,           /*    analyze water age                */
		TRACE
	};        /*    trace % of flow from a source    */

	enum NodeType                  /* Type of node:                       */
	{
		JUNC,          /*    junction                         */
		RESERV,        /*    reservoir                        */
		TANK
	};         /*    tank                             */

	enum LinkType                  /* Type of link:                       */
	{
		CV,           /*    pipe with check valve            */
		PIPE,         /*    regular pipe                     */
		PUMP,         /*    pump                             */
		PRV,          /*    pressure reducing valve          */
		PSV,          /*    pressure sustaining valve        */
		PBV,          /*    pressure breaker valve           */
		FCV,          /*    flow control valve               */
		TCV,          /*    throttle control valve           */
		GPV
	};         /*    general purpose valve            */

	enum CurveType                /* Type of curve:                       */
	{
		V_CURVE,     /*    volume curve                      */
		P_CURVE,     /*    pump curve                        */
		E_CURVE,     /*    efficiency curve                  */
		H_CURVE
	};    /*    head loss curve                   */

	enum PumpType                  /* Type of pump curve:                 */
	{
		CONST_HP,      /*    constant horsepower              */
		POWER_FUNC,    /*    power function                   */
		CUSTOM,        /*    user-defined custom curve        */
		NOCURVE
	};

	enum SourceType                /* Type of source quality input        */
	{
		CONCEN,        /*    inflow concentration             */
		MASS,          /*    mass inflow booster              */
		SETPOINT,      /*    setpoint booster                 */
		FLOWPACED
	};    /*    flow paced booster               */

	enum ControlType               /* Control condition type:             */
	{
		LOWLEVEL,      /*    act when grade below set level   */
		HILEVEL,       /*    act when grade above set level   */
		TIMER,         /*    act when set time reached        */
		TIMEOFDAY
	};    /*    act when time of day occurs      */

	enum StatType                  /* Link/Tank status:                   */
	{
		XHEAD,        /*   pump cannot deliver head (closed) */
		TEMPCLOSED,   /*   temporarily closed                */
		CLOSED,       /*   closed                            */
		OPEN,         /*   open                              */
		ACTIVE,       /*   valve active (partially open)     */
		XFLOW,        /*   pump exceeds maximum flow         */
		XFCV,         /*   FCV cannot supply flow            */
		XPRESSURE,    /*   valve cannot supply pressure      */
		FILLING,      /*   tank filling                      */
		EMPTYING
	};    /*   tank emptying                     */

	enum FormType                  /* Head loss formula:                  */
	{
		HW,           /*   Hazen-Williams                    */
		DW,           /*   Darcy-Weisbach                    */
		CM
	};          /*   Chezy-Manning                     */

	enum UnitsType                 /* Unit system:                        */
	{
		US,           /*   US                                */
		SI
	};          /*   SI (metric)                       */

	enum FlowUnitsType             /* Flow units:                         */
	{
		CFS,          /*   cubic feet per second             */
		GPM,          /*   gallons per minute                */
		MGD,          /*   million gallons per day           */
		IMGD,         /*   imperial million gal. per day     */
		AFD,          /*   acre-feet per day                 */
		LPS,          /*   liters per second                 */
		LPM,          /*   liters per minute                 */
		MLD,          /*   megaliters per day                */
		CMH,          /*   cubic meters per hour             */
		CMD
	};         /*   cubic meters per day              */

	enum PressUnitsType            /* Pressure units:                     */
	{
		PSI,          /*   pounds per square inch            */
		KPA,          /*   kiloPascals                       */
		METERS
	};      /*   meters                            */

	enum RangeType                 /* Range limits:                       */
	{
		LOW,          /*   lower limit                       */
		HI,           /*   upper limit                       */
		PREC
	};        /*   precision                         */

	enum MixType                   /* Tank mixing regimes                 */
	{
		MIX1,         /*   1-compartment model               */
		MIX2,         /*   2-compartment model               */
		FIFO,         /*   First in, first out model         */
		LIFO
	};        /*   Last in, first out model          */

	enum TstatType                 /* Time series statistics              */
	{
		SERIES,       /*   none                              */
		AVG,          /*   time-averages                     */
		MIN,          /*   minimum values                    */
		MAX,          /*   maximum values                    */
		RANGE
	};       /*   max - min values                  */

	enum FieldType                 /* Network variables:                  */
	{
		ELEV,         /*   nodal elevation                   */
		DEMAND,       /*   nodal demand flow                 */
		HEAD,         /*   nodal hydraulic head              */
		PRESSURE,     /*   nodal pressure                    */
		QUALITY,      /*   nodal water quality               */

		LENGTH,       /*   link length                       */
		DIAM,         /*   link diameter                     */
		FLOW,         /*   link flow rate                    */
		VELOCITY,     /*   link flow velocity                */
		HEADLOSS,     /*   link head loss                    */
		LINKQUAL,     /*   avg. water quality in link        */
		STATUS,       /*   link status                       */
		SETTING,      /*   pump/valve setting                */
		REACTRATE,    /*   avg. reaction rate in link        */
		FRICTION,     /*   link friction factor              */

		POWER,        /*   pump power output                 */
		TIME,         /*   simulation time                   */
		VOLUME,       /*   tank volume                       */
		CLOCKTIME,    /*   simulation time of day            */
		FILLTIME,     /*   time to fill a tank               */
		DRAINTIME
	};   /*   time to drain a tank              */

	enum SectType {
		_TITLE, _JUNCTIONS, _RESERVOIRS, _TANKS, _PIPES, _PUMPS,
		_VALVES, _CONTROLS, _RULES, _DEMANDS, _SOURCES, _EMITTERS,
		_PATTERNS, _CURVES, _QUALITY, _STATUS, _ROUGHNESS, _ENERGY,
		_REACTIONS, _MIXING, _REPORT, _TIMES, _OPTIONS,
		_COORDS, _VERTICES, _LABELS, _BACKDROP, _TAGS, _END
	};

	enum HdrType                    /* Type of table heading   */
	{
		STATHDR,      /*  Hydraulic Status       */
		ENERHDR,      /*  Energy Usage           */
		NODEHDR,      /*  Node Results           */
		LINKHDR
	};     /*  Link Results           */


	/*rules*/
	enum    Rulewords { r_RULE, r_IF, r_AND, r_OR, r_THEN, r_ELSE, r_PRIORITY, r_ERROR };
	//char    *Ruleword[] = { w_RULE,w_IF,w_AND,w_OR,w_THEN,w_ELSE,w_PRIORITY,NULL };

	enum    Varwords {
		r_DEMAND, r_HEAD, r_GRADE, r_LEVEL, r_PRESSURE,
		r_FLOW, r_STATUS, r_SETTING, r_POWER, r_TIME,
		r_CLOCKTIME, r_FILLTIME, r_DRAINTIME
	};
	/*char    *Varword[] = { w_DEMAND, w_HEAD, w_GRADE, w_LEVEL, w_PRESSURE,
		w_FLOW, w_STATUS, w_SETTING, w_POWER,w_TIME,
		w_CLOCKTIME,w_FILLTIME,w_DRAINTIME, NULL };*/

	enum    Objects {
		r_JUNC, r_RESERV, r_TANK, r_PIPE, r_PUMP, r_VALVE,
		r_NODE, r_LINK, r_SYSTEM
	};
	/*char    *Object[] = { w_JUNC,w_RESERV,w_TANK,w_PIPE,w_PUMP,w_VALVE,
		w_NODE,w_LINK,w_SYSTEM,NULL };*/

		/* NOTE: place "<=" & ">=" before "<" & ">" so that findmatch() works correctly. */
	enum    Operators { EQ, NE, LE, GE, LT, GT, IS, NOT, BELOW, ABOVE };
	//char    *Operator[] = { "=","<>","<=",">=","<",">",w_IS,w_NOT,w_BELOW,w_ABOVE,NULL };

	enum    Values { IS_NUMBER, IS_OPEN, IS_CLOSED, IS_ACTIVE };
	//char    *Value[] = { "XXXX",   w_OPEN, w_CLOSED, w_ACTIVE,NULL };

}
#endif // !EPANET_TYPES_H