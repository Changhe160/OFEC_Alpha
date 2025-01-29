/*
*******************************************************************************

EPANET.C -- Hydraulic & Water Quality Simulator for Water Distribution Networks

VERSION:    2.00
DATE:       5/30/00
			9/7/00
			10/25/00
			3/1/01
			11/19/01
			6/24/02
			8/15/07    (2.00.11)
			2/14/08    (2.00.12)
AUTHOR:     L. Rossman
			US EPA - NRMRL

EPANET performs extended period hydraulic and water quality analysis of
looped, pressurized piping networks. The program consists of the
following code modules:

	EPANET.C  -- main module providing supervisory control
	INPUT1.C  -- controls processing of input data
	INPUT2.C  -- reads data from input file
	INPUT3.C  -- parses Solution lines of input data
	INPFILE.C -- saves modified input data to a text file
	RULES.C   -- implements rule-based control of piping system
	HYDRAUL.C -- computes extended period hydraulic behavior
	QUALITY.C -- tracks transport & fate of water quality
	OUTPUT.C  -- handles transfer of data to and from binary files
	REPORT.C  -- handles reporting of results to text file
	SMATRIX.C -- sparse matrix linear equation solver routines
	MEMPOOL.C -- memory allocation routines
	HASH.C    -- hash table routines

The program can be compiled as either a stand-alone console application
or as a dynamic link library (DLL) of function calls depending on whether
the macro identifier 'DLL' is defined or not.

See TOOLKIT.H for function prototypes of exported DLL functions
See FUNCS.H for prototypes of all other functions
See TYPES.H for declaration of global constants and data structures
See VARS.H for declaration of global variables
See TEXT.H for declaration of all string constants
See ENUMSTXT.H for assignment of string constants to enumerated types

The following naming conventions are used in all modules of this program:
1. Names of exportable functions in the DLL begin with the "EN" prefix.
2. All other function names are lowercase.
3. Global variable names begin with an uppercase letter.
4. Local variable names are all lowercase.
5. Declared constants and enumerated values defined in TYPES.H are
   all uppercase.
6. String constants defined in TEXT.H begin with a lower case character
   followed by an underscore and then all uppercase characters (e.g.
   t_HEADLOSS)

--------------------------------------------------------------------------

This is the main module of the EPANET program. It uses a series of
functions, all beginning with the letters EN, to control program behavior.
See the main() and ENepanet() functions below for the simplest example of
these.

This module calls the following functions that reside in other modules:
   RULES.C
	 initrules()
	 allocrules()
	 closerules()
   INPUT1.C
	 get_data()
	 initreport()
   INPUT2.C
	 netsize()
	 setreport()
   HYDRAUL.C
	 openhyd()
	 inithyd()
	 runhyd()
	 nexthyd()
	 closehyd()
	 resistance()
	 tankvolume()
	 getenergy()
	 setlinkstatus()
	 setlinksetting()
   QUALITY.C
	 openqual()
	 initqual()
	 runqual()
	 nextqual()
	 stepqual()
	 closequal()
   REPORT.C
	 writeline()
	 writelogo()
	 writereport()
   HASH.C
	 HTcreate()
	 HTfind()
	 HTfree()

The macro ERRCODE(x) is defined in TYPES.H. It says if the current
value of the error code variable (errcode) is not fatal (< 100) then
execute function x and set the error code equal to its return value.

*******************************************************************************
*/

#include "epanet.h"
#include <cstring>
#include <cmath>
#include <ctime>

/*
****************************************************

      String Constants for EPANET Program

VERSION:    2.00
DATE:       5/8/00
            10/25/00
            8/15/07     (2.00.11)
            2/14/08    (2.00.12)
AUTHOR:     L. Rossman
            US EPA - NRMRL

****************************************************/

// --- define WINDOWS
#undef WINDOWS
#ifdef _WIN32
#define WINDOWS
#endif
#ifdef __WIN32__
#define WINDOWS
#endif


/* ------------ Keyword Dictionary ---------- */
#define   w_USE         "USE"
#define   w_SAVE        "SAVE"

#define   w_NONE        "NONE"
#define   w_ALL         "ALL"

#define   w_CHEM        "CHEM"
#define   w_AGE         "AGE"
#define   w_TRACE       "TRACE"

#define   w_SYSTEM      "SYST"
#define   w_JUNC        "Junc"
#define   w_RESERV      "Reser"
#define   w_TANK        "Tank"
#define   w_CV          "CV"
#define   w_PIPE        "Pipe"
#define   w_PUMP        "Pump"
#define   w_VALVE       "Valve"
#define   w_PRV         "PRV"
#define   w_PSV         "PSV"
#define   w_PBV         "PBV"
#define   w_FCV         "FCV"
#define   w_TCV         "TCV"
#define   w_GPV         "GPV"

#define   w_OPEN        "OPEN"
#define   w_CLOSED      "CLOSED"
#define   w_ACTIVE      "ACTIVE"
#define   w_TIME        "TIME"
#define   w_ABOVE       "ABOVE"
#define   w_BELOW       "BELOW"
#define   w_PRECISION   "PREC"
#define   w_IS          "IS"
#define   w_NOT         "NOT"

#define   w_ADD         "ADD"
#define   w_MULTIPLY    "MULT"

#define   w_LIMITING    "LIMIT"
#define   w_ORDER       "ORDER"
#define   w_GLOBAL      "GLOB"
#define   w_BULK        "BULK"
#define   w_WALL        "WALL"

#define   w_PAGE        "PAGE"
#define   w_STATUS      "STATUS"
#define   w_SUMMARY     "SUMM"
#define   w_MESSAGES    "MESS"
#define   w_ENERGY      "ENER"
#define   w_NODE        "NODE"
#define   w_LINK        "LINK"
#define   w_FILE        "FILE"
#define   w_YES         "YES"
#define   w_NO          "NO"
#define   w_FULL        "FULL"

#define   w_HW          "H-W"
#define   w_DW          "D-W"
#define   w_CM          "C-M"

#define   w_CFS         "CFS"
#define   w_GPM         "GPM"
#define   w_MGD         "MGD"
#define   w_IMGD        "IMGD"
#define   w_AFD         "AFD"
#define   w_LPS         "LPS"
#define   w_LPM         "LPM"
#define   w_MLD         "MLD"
#define   w_CMH         "CMH"
#define   w_CMD         "CMD"
#define   w_SI          "SI"

#define   w_PSI         "PSI"
#define   w_KPA         "KPA"
#define   w_METERS      "METERS"

#define   w_ELEV        "ELEV"
#define   w_DEMAND      "DEMA"
#define   w_HEAD        "HEAD"
#define   w_PRESSURE    "PRES"
#define   w_QUALITY     "QUAL"

#define   w_DIAM        "DIAM"
#define   w_FLOW        "FLOW"
#define   w_ROUGHNESS   "ROUG"
#define   w_VELOCITY    "VELO"
#define   w_HEADLOSS    "HEADL"
#define   w_SETTING     "SETT"
#define   w_POWER       "POWE"
#define   w_VOLUME      "VOLU"
#define   w_CLOCKTIME   "CLOCKTIME"
#define   w_FILLTIME    "FILL"
#define   w_DRAINTIME   "DRAI"
#define   w_GRADE       "GRADE"
#define   w_LEVEL       "LEVEL"

#define   w_DURATION    "DURA"
#define   w_HYDRAULIC   "HYDR"
#define   w_MINIMUM     "MINI"
#define   w_PATTERN     "PATT"
#define   w_REPORT      "REPO"
#define   w_START       "STAR"

#define   w_UNITS       "UNIT"
#define   w_MAP         "MAP"
#define   w_VERIFY      "VERI"
#define   w_VISCOSITY   "VISC"
#define   w_DIFFUSIVITY "DIFF"
#define   w_SPECGRAV    "SPEC"
#define   w_TRIALS      "TRIAL"
#define   w_ACCURACY    "ACCU"
#define   w_SEGMENTS    "SEGM"
#define   w_TOLERANCE   "TOLER"
#define   w_EMITTER     "EMIT"

#define   w_PRICE       "PRICE"
#define   w_DMNDCHARGE  "DEMAN"

#define   w_HTOL        "HTOL"
#define   w_QTOL        "QTOL"
#define   w_RQTOL       "RQTOL"
#define   w_CHECKFREQ   "CHECKFREQ"
#define   w_MAXCHECK    "MAXCHECK"
#define   w_DAMPLIMIT   "DAMPLIMIT"                                            //(2.00.12 - LR)

#define   w_SECONDS     "SEC"
#define   w_MINUTES     "MIN"
#define   w_HOURS       "HOU"
#define   w_DAYS        "DAY"
#define   w_AM          "AM"
#define   w_PM          "PM"

#define   w_CONCEN      "CONCEN"
#define   w_MASS        "MASS"
#define   w_SETPOINT    "SETPOINT"
#define   w_FLOWPACED   "FLOWPACED"

#define   w_PATTERN     "PATT"
#define   w_CURVE       "CURV"

#define   w_EFFIC       "EFFI"
#define   w_HEAD        "HEAD"
#define   w_POWER       "POWE"
#define   w_SPEED       "SPEE"

#define   w_MIXED       "MIXED"
#define   w_2COMP       "2COMP"
#define   w_FIFO        "FIFO"
#define   w_LIFO        "LIFO"

#define   w_STATISTIC   "STAT"
#define   w_AVG         "AVERAGE"
#define   w_MIN         "MINIMUM"
#define   w_MAX         "MAXIMUM"
#define   w_RANGE       "RANGE"

#define   w_UNBALANCED  "UNBA"
#define   w_STOP        "STOP"
#define   w_CONTINUE    "CONT"

#define   w_RULE        "RULE"
#define   w_IF          "IF"
#define   w_AND         "AND"
#define   w_OR          "OR"
#define   w_THEN        "THEN"
#define   w_ELSE        "ELSE"
#define   w_PRIORITY    "PRIO"

/* ---------Input Section Names ---------- */
#define   s_TITLE       "[TITL"
#define   s_JUNCTIONS   "[JUNC"
#define   s_RESERVOIRS  "[RESE"
#define   s_TANKS       "[TANK"
#define   s_PIPES       "[PIPE"
#define   s_PUMPS       "[PUMP"
#define   s_VALVES      "[VALV"
#define   s_CONTROLS    "[CONT"
#define   s_RULES       "[RULE"
#define   s_DEMANDS     "[DEMA"
#define   s_SOURCES     "[SOUR"
#define   s_EMITTERS    "[EMIT"
#define   s_PATTERNS    "[PATT"
#define   s_CURVES      "[CURV"
#define   s_QUALITY     "[QUAL"
#define   s_STATUS      "[STAT"
#define   s_ROUGHNESS   "[ROUG"
#define   s_ENERGY      "[ENER"
#define   s_REACTIONS   "[REAC"
#define   s_MIXING      "[MIXI"
#define   s_REPORT      "[REPO"
#define   s_TIMES       "[TIME"
#define   s_OPTIONS     "[OPTI"
#define   s_COORDS      "[COOR"
#define   s_VERTICES    "[VERT"
#define   s_LABELS      "[LABE"
#define   s_BACKDROP    "[BACK"
#define   s_TAGS        "[TAGS"
#define   s_END         "[END"

/* ---------------- Units ---------------- */
/*** Limit units to MAXID or less characters ***/
#define   u_CFS         "cfs"
#define   u_GPM         "gpm"
#define   u_AFD         "a-f/d"
#define   u_MGD         "mgd"
#define   u_IMGD        "Imgd"
#define   u_LPS         "L/s"
#define   u_LPM         "Lpm"
#define   u_CMH         "m3/h"
#define   u_CMD         "m3/d"
#define   u_MLD         "ML/d"
#define   u_MGperL      "mg/L"
#define   u_UGperL      "ug/L"
#define   u_HOURS       "hrs"
#define   u_MINUTES     "min"
#define   u_PERCENT     "% from"
#define   u_METERS      "m"
#define   u_MMETERS     "mm"
#define   u_MperSEC     "m/s"
#define   u_SQMperSEC   "sq m/sec"
#define   u_per1000M    "/1000m"
#define   u_KW          "kw"
#define   u_FEET        "ft"
#define   u_INCHES      "in"
#define   u_PSI         "psi"
#define   u_KPA         "kPa"
#define   u_FTperSEC    "fps"
#define   u_SQFTperSEC  "sq ft/sec"
#define   u_per1000FT   "/1000ft"
#define   u_HP          "hp"

/* -------------- Curve Types ----------------- */
#define   c_HEADLOSS    "HEADLOSS"
#define   c_PUMP        "PUMP"
#define   c_EFFIC       "EFFIC"
#define   c_VOLUME      "VOLUME"

/* ------------------  Text Phrases ------------------- */
#define   t_ABOVE       "above"
#define   t_BELOW       "below"
#define   t_HW          "Hazen-Williams"
#define   t_DW          "Darcy-Weisbach"
#define   t_CM          "Chezy-Manning"
#define   t_CHEMICAL    "Chemical"
#define   t_XHEAD       "closed because cannot deliver head"
#define   t_TEMPCLOSED  "temporarily closed"
#define   t_CLOSED      "closed"
#define   t_OPEN        "open"
#define   t_ACTIVE      "active"
#define   t_XFLOW       "open but exceeds maximum flow"
#define   t_XFCV        "open but cannot deliver flow"
#define   t_XPRESSURE   "open but cannot deliver pressure"
#define   t_FILLING     "filling"
#define   t_EMPTYING    "emptying"

#define   t_ELEV        "Elevation"
#define   t_DEMAND      "Demand"
#define   t_HEAD        "Head"
#define   t_PRESSURE    "Pressure"
#define   t_QUALITY     "Quality"
#define   t_LENGTH      "Length"
#define   t_DIAM        "Diameter"
#define   t_FLOW        "Flow"
#define   t_VELOCITY    "Velocity"
#define   t_HEADLOSS    "Headloss"
#define   t_LINKQUAL    "Quality"
#define   t_LINKSTATUS  "State"
#define   t_SETTING     "Setting"
#define   t_REACTRATE   "Reaction"
#define   t_FRICTION    "F-Factor"

#define   t_NODEID      "Node"
#define   t_LINKID      "Link"
#define   t_PERDAY      "/day"

#define   t_JUNCTION    "Junction"
#define   t_RESERVOIR   "Reservoir"
#define   t_TANK        "Tank"
#define   t_PIPE        "Pipe"
#define   t_PUMP        "Pump"
#define   t_VALVE       "Valve"
#define   t_CONTROL     "Control"
#define   t_RULE        "Rule"
#define   t_DEMANDFOR   "Demand for Node"
#define   t_SOURCE      "Source"
#define   t_EMITTER     "Emitter"
#define   t_PATTERN     "Pattern"
#define   t_CURVE       "Curve"
#define   t_STATUS      "Status"
#define   t_ROUGHNESS   "Roughness"
#define   t_ENERGY      "Energy"
#define   t_REACTION    "Reaction"
#define   t_MIXING      "Mixing"
#define   t_REPORT      "Report"
#define   t_TIME        "Times"
#define   t_OPTION      "Options"
#define   t_RULES_SECT  "[RULES] section"
#define   t_HALTED      " EXECUTION HALTED."
#define   t_FUNCCALL    "function call"
#define   t_CONTINUED   " (continued)"
#define   t_perM3       "  /m3"
#define   t_perMGAL     "/Mgal"
#define   t_DIFFER      "DIFFERENTIAL"


/* ------------------ Format Messages ------------------*/
#define LOGO1  \
"******************************************************************"
#define LOGO2  \
"*                           E P A N E T                          *"
#define LOGO3  \
"*                   Hydraulic and Water Quality                  *"
#define LOGO4  \
"*                   Analysis for Pipe Networks                   *"
#define LOGO5  \
"*                         Version 2.00.12                        *"           //(2.00.12 - LR)
#define LOGO6  \
"******************************************************************"
#define FMT01  "\n... EPANET Version 2.0\n"
#define FMT02  "\n  o Retrieving network data"
#define FMT03  "\n Correct syntax is:\n epanet <input file> <output file>\n"
#define FMT04  "\n    Cannot use duplicate file names."
#define FMT05  "\n    Cannot open input file "
#define FMT06  "\n    Cannot open report file "
#define FMT07  "\n    Cannot open output file "
#define FMT08  "\n    Cannot open temporary output file"
#define FMT09  "\n\n... EPANET completed.\n"
#define FMT10  "\n\n... EPANET completed. There are warnings.\n"
#define FMT11  "\n\n... EPANET completed. There are errors.\n"
#define FMT14  "\n  o Computing hydraulics at hour "
#define FMT15  "\n  o Computing water quality at hour "
#define FMT16  "\n  o Transferring results to file"
#define FMT17  "\n  o Writing output report to "
#define FMT18  "  Page 1                                    "
#define FMT19  "    Input Data File ................... %s"
#define FMT20  "    Number of Junctions................ %-d"
#define FMT21a "    Number of Reservoirs............... %-d"
#define FMT21b "    Number of Tanks ................... %-d"
#define FMT22  "    Number of Pipes ................... %-d"
#define FMT23  "    Number of Pumps ................... %-d"
#define FMT24  "    Number of Valves .................. %-d"
#define FMT25  "    Headloss Formula .................. %s"
#define FMT26  "    Hydraulic Timestep ................ %-.2f %s"
#define FMT27  "    Hydraulic Accuracy ................ %-.6f"

#define FMT27a "    Status Check Frequency ............ %-d"                   //(2.00.12 - LR)
#define FMT27b "    Maximum Trials Checked ............ %-d"                   //(2.00.12 - LR)
#define FMT27c "    Damping Limit Threshold ........... %-.6f"                 //(2.00.12 - LR)

#define FMT28  "    Maximum Trials .................... %-d"
#define FMT29  "    Quality Analysis .................. None"
#define FMT30  "    Quality Analysis .................. %s"
#define FMT31  "    Quality Analysis .................. Trace From Node %s"
#define FMT32  "    Quality Analysis .................. Age"
#define FMT33  "    Water Quality Time Step ........... %-.2f min"
#define FMT34  "    Water Quality Tolerance ........... %-.2f %s"
#define FMT36  "    Specific Gravity .................. %-.2f"
#define FMT37a "    Relative Kinematic Viscosity ...... %-.2f"
#define FMT37b "    Relative Chemical Diffusivity ..... %-.2f"
#define FMT38  "    Demand Multiplier ................. %-.2f"
#define FMT39  "    Total Duration .................... %-.2f %s"
#define FMT40  "    Reporting Criteria:"
#define FMT41  "       No Nodes"
#define FMT42  "       All Nodes"
#define FMT43  "       Selected Nodes"
#define FMT44  "       No Links"
#define FMT45  "       All Links"
#define FMT46  "       Selected Links"
#define FMT47  "       with %s below %-.2f %s"
#define FMT48  "       with %s above %-.2f %s"

/* ---------- Status Report Format Strings ------------ */
#define FMT49  "Hydraulic Status:"

/*** Updated 6/24/02 ***/
#define FMT50  "%10s: Tank %s is %s at %-.2f %s"
#define FMT51  "%10s: Reservoir %s is %s"
#define FMT52  "%10s: %s %s %s"
#define FMT53  "%10s: %s %s changed from %s to %s"
#define FMT54  "%10s: %s %s changed by %s %s control"
#define FMT55  "%10s: %s %s changed by timer control"
#define FMT56  "            %s %s setting changed to %-.2f"
#define FMT57  "            %s %s switched from %s to %s"
#define FMT58  "%10s: Balanced after %-d trials"
#define FMT59  "%10s: Unbalanced after %-d trials (flow change = %-.6f)"

#define FMT60a "            Max. flow imbalance is %.4f %s at Node %s"         //(2.00.12 - LR)
#define FMT60b "            Max. head imbalance is %.4f %s at Link %s"         //(2.00.12 - LR)

#define FMT61  "%10s: Valve %s caused ill-conditioning"
#define FMT62  "%10s: System ill-conditioned at node %s"
#define FMT63  "%10s: %s %s changed by rule %s"
#define FMT64  "%10s: Balancing the network:"
#define FMT65  "            Trial %2d: relative flow change = %-.6f"
/*** End of update ***/

/* -------------------- Energy Report Table ------------------- */
#define FMT71  "Energy Usage:"
#define FMT72  \
        "           Usage   Avg.     Kw-hr      Avg.      Peak      Cost"
#define FMT73  \
        "Pump      Factor Effic.     %s        Kw        Kw      /day"
#define FMT74  "%38s Demand Charge: %9.2f"
#define FMT75  "%38s Total Cost:    %9.2f"

/* -------------------- Node Report Table --------------------- */
#define FMT76  "%s Node Results:"
#define FMT77  "Node Results:"
#define FMT78  "Node Results at %s hrs:"

/* -------------------- Link Report Table --------------------- */
#define FMT79  "%s Link Results:"
#define FMT80  "Link Results:"
#define FMT81  "Link Results at %s hrs:"
#define FMT82  "\n\f\n  Page %-d    %60.60s\n"

/* ------------------- Progress Messages ---------------------- */
#define FMT100 "Retrieving network data..."
#define FMT101 "Computing hydraulics at hour %s"
#define FMT102 "Computing water quality at hour %s"
#define FMT103 "Saving results to file..."
#define FMT104 "Analysis begun %s"
#define FMT105 "Analysis ended %s"

/*------------------- Error Messages --------------------*/
#define ERR101 "System Error 101: insufficient memory available."
#define ERR102 "System Error 102: no network data available."
#define ERR103 "System Error 103: hydraulics not initialized."
#define ERR104 "System Error 104: no hydraulics for water quality analysis."
#define ERR105 "System Error 105: water quality not initialized."
#define ERR106 "System Error 106: no results saved to report on."
#define ERR107 "System Error 107: hydraulics supplied from external file."
#define ERR108 "System Error 108: cannot use external file while hydraulics solver is active."
#define ERR109 "System Error 109: cannot change time parameter when solver is active."
#define ERR110 "System Error 110: cannot solve network hydraulic equations."
#define ERR120 "System Error 120: cannot solve water quality transport equations."

#define ERR200 "Input Error 200: one or more errors in input file."
#define ERR201 \
 "Input Error 201: syntax error in following line of [%s] section:"
#define ERR202 "Input Error 202: %s %s contains illegal numeric value."
#define ERR203 "Input Error 203: %s %s refers to undefined node."
#define ERR204 "Input Error 204: %s %s refers to undefined link."
#define ERR205 "Input Error 205: %s %s refers to undefined time pattern."
#define ERR206 "Input Error 206: %s %s refers to undefined curve."
#define ERR207 "Input Error 207: %s %s attempts to control a CV."

#define ERR208 "Input Error 208: %s specified for undefined Node %s."
#define ERR209 "Input Error 209: illegal %s value for Node %s."
#define ERR210 "Input Error 210: %s specified for undefined Link %s."
#define ERR211 "Input Error 211: illegal %s value for Link %s."
#define ERR212 "Input Error 212: trace node %.0s %s is undefined."
#define ERR213 "Input Error 213: illegal option value in [%s] section:"
#define ERR214 \
 "Input Error 214: following line of [%s] section contains too many characters:"
#define ERR215 "Input Error 215: %s %s is a duplicate ID."
#define ERR216 "Input Error 216: %s data specified for undefined Pump %s."
#define ERR217 "Input Error 217: invalid %s data for Pump %s."
#define ERR219 "Input Error 219: %s %s illegally connected to a tank."
#define ERR220 "Input Error 220: %s %s illegally connected to another valve."

/*** Updated on 10/25/00 ***/
#define ERR222 "Input Error 222: %s %s has same start and end nodes."

#define ERR223 "Input Error 223: not enough nodes in network"
#define ERR224 "Input Error 224: no tanks or reservoirs in network."
#define ERR225 "Input Error 225: invalid lower/upper levels for Tank %s."
#define ERR226 "Input Error 226: no head curve supplied for Pump %s."
#define ERR227 "Input Error 227: invalid head curve for Pump %s."
#define ERR230 "Input Error 230: Curve %s has nonincreasing x-values."
#define ERR233 "Input Error 233: Node %s is unconnected."
#define ERR240 "Input Error 240: %s %s refers to undefined source."
#define ERR241 "Input Error 241: %s %s refers to undefined control."
#define ERR250 "Input Error 250: function call contains invalid format."
#define ERR251 "Input Error 251: function call contains invalid parameter code."

#define ERR301 "File Error 301: identical file names."
#define ERR302 "File Error 302: cannot open input file."
#define ERR303 "File Error 303: cannot open report file."
#define ERR304 "File Error 304: cannot open binary output file."
#define ERR305 "File Error 305: cannot open hydraulics file."
#define ERR306 "File Error 306: hydraulics file does not match network data."
#define ERR307 "File Error 307: cannot read hydraulics file."
#define ERR308 "File Error 308: cannot save results to file."
#define ERR309 "File Error 309: cannot save results to report file."

#define R_ERR201 "Input Error 201: syntax error in following line of "
#define R_ERR202 "Input Error 202: illegal numeric value in following line of "
#define R_ERR203 "Input Error 203: undefined node in following line of "
#define R_ERR204 "Input Error 204: undefined link in following line of "
#define R_ERR207 "Input Error 207: attempt to control a CV in following line of "

#define R_ERR221 "Input Error 221: mis-placed clause in following line of "

/*-------------------- Specific Warning Messages -------------------------*/
#define WARN01 "WARNING: System unbalanced at %s hrs."
#define WARN02 \
"WARNING: Maximum trials exceeded at %s hrs. System may be unstable."
#define WARN03a "WARNING: Node %s disconnected at %s hrs"
#define WARN03b "WARNING: %d additional nodes disconnected at %s hrs"
#define WARN03c "WARNING: System disconnected because of Link %s" 
#define WARN04  "WARNING: Pump %s %s at %s hrs."
#define WARN05  "WARNING: %s %s %s at %s hrs."
#define WARN06  "WARNING: Negative pressures at %s hrs."

/*-------------------- General Warning Messages -------------------------*/
#define WARN1 "WARNING: System hydraulically unbalanced."
#define WARN2 "WARNING: System may be hydraulically unstable."
#define WARN3 "WARNING: System disconnected."
#define WARN4 "WARNING: Pumps cannot deliver enough flow or head."
#define WARN5 "WARNING: Valves cannot deliver enough flow."
#define WARN6 "WARNING: System has negative pressures."

/***********************************************************************

ENUMSTXT.H -- Text strings for enumerated data types in EPANET

VERSION : 2.00
DATE : 5 / 8 / 00
AUTHOR : L.Rossman
US EPA - NRMRL

* *********************************************************************/

const char *NodeTxt[] = { t_JUNCTION,
						   t_RESERVOIR,
						   t_TANK };

const char *LinkTxt[] = { w_CV,
						   w_PIPE,
						   w_PUMP,
						   w_PRV,
						   w_PSV,
						   w_PBV,
						   w_FCV,
						   w_TCV,
						   w_GPV };

const char *StatTxt[] = { t_XHEAD,
						   t_TEMPCLOSED,
						   t_CLOSED,
						   t_OPEN,
						   t_ACTIVE,
						   t_XFLOW,
						   t_XFCV,
						   t_XPRESSURE,
						   t_FILLING,
						   t_EMPTYING };

const char *FormTxt[] = { w_HW,
						   w_DW,
						   w_CM };

const char *RptFormTxt[] = { t_HW,
						   t_DW,
						   t_CM };

const char *RptFlowUnitsTxt[] = { u_CFS,
						   u_GPM,
						   u_MGD,
						   u_IMGD,
						   u_AFD,
						   u_LPS,
						   u_LPM,
						   u_MLD,
						   u_CMH,
						   u_CMD };

const char *FlowUnitsTxt[] = { w_CFS,
						   w_GPM,
						   w_MGD,
						   w_IMGD,
						   w_AFD,
						   w_LPS,
						   w_LPM,
						   w_MLD,
						   w_CMH,
						   w_CMD };

const char *PressUnitsTxt[] = { w_PSI,
						   w_KPA,
						   w_METERS };

const char *QualTxt[] = { w_NONE,
						   w_CHEM,
						   w_AGE,
						   w_TRACE };


const char *SourceTxt[] = { w_CONCEN,
						   w_MASS,
						   w_SETPOINT,
						   w_FLOWPACED };

const char *ControlTxt[] = { w_BELOW,
						   w_ABOVE,
						   w_TIME,
						   w_CLOCKTIME };

const char *TstatTxt[] = { w_NONE,
						   w_AVG,
						   w_MIN,
						   w_MAX,
						   w_RANGE };

const char *MixTxt[] = { w_MIXED,
						   w_2COMP,
						   w_FIFO,
						   w_LIFO,
						   nullptr };

const char *RptFlagTxt[] = { w_NO,
						   w_YES,
						   w_FULL };

const char *SectTxt[] = { s_TITLE,     s_JUNCTIONS, s_RESERVOIRS,
						   s_TANKS,     s_PIPES,     s_PUMPS,
						   s_VALVES,    s_CONTROLS,  s_RULES,
						   s_DEMANDS,   s_SOURCES,   s_EMITTERS,
						   s_PATTERNS,  s_CURVES,    s_QUALITY,
						   s_STATUS,    s_ROUGHNESS, s_ENERGY,
						   s_REACTIONS, s_MIXING,    s_REPORT,
						   s_TIMES,     s_OPTIONS,   s_COORDS,
						   s_VERTICES,  s_LABELS,    s_BACKDROP,
						   s_TAGS,      s_END,
						   nullptr };

const char *RptSectTxt[] = { nullptr,       t_JUNCTION, t_RESERVOIR,
						   t_TANK,     t_PIPE,     t_PUMP,
						   t_VALVE,    t_CONTROL,  t_RULE,
						   t_DEMANDFOR,t_SOURCE,   t_EMITTER,
						   t_PATTERN,  t_CURVE,    t_QUALITY,
						   t_STATUS,   t_ROUGHNESS,t_ENERGY,
						   t_REACTION, t_MIXING,   t_REPORT,
						   t_TIME,     t_OPTION };

const char *Fldname[] = { t_ELEV,      t_DEMAND,    t_HEAD,
						   t_PRESSURE,  t_QUALITY,   t_LENGTH,
						   t_DIAM,      t_FLOW,      t_VELOCITY,
						   t_HEADLOSS,  t_LINKQUAL,  t_LINKSTATUS,
						   t_SETTING,   t_REACTRATE, t_FRICTION,
						   "", "", "", "", "", "", nullptr };

const char *LogoTxt[] = { LOGO1,LOGO2,LOGO3,LOGO4,LOGO5,LOGO6,nullptr };

/*rules*/
const char *Ruleword[] = { w_RULE,w_IF,w_AND,w_OR,w_THEN,w_ELSE,w_PRIORITY,nullptr };
const char *Varword[] = { w_DEMAND, w_HEAD, w_GRADE, w_LEVEL, w_PRESSURE,
	w_FLOW, w_STATUS, w_SETTING, w_POWER,w_TIME,
	w_CLOCKTIME,w_FILLTIME,w_DRAINTIME, nullptr };
const char *Object[] = { w_JUNC,w_RESERV,w_TANK,w_PIPE,w_PUMP,w_VALVE,
	w_NODE,w_LINK,w_SYSTEM,nullptr };
const char *Operator[] = { "=","<>","<=",">=","<",">",w_IS,w_NOT,w_BELOW,w_ABOVE,nullptr };
const char *Value[] = { "XXXX",   w_OPEN, w_CLOSED, w_ACTIVE,nullptr };

/*
***********************************************************************

TYPES.H -- Global constants and data types for EPANET program

VERSION:    2.00
DATE:       5/8/00
			9/7/00
			10/25/00
			3/1/01
			12/6/01
			6/24/02
			8/15/07    (2.00.11)
			2/14/08    (2.00.12)
AUTHOR:     L. Rossman
			US EPA - NRMRL

**********************************************************************
*/

/*********************************************************/
/* All floats have been re-declared as doubles (7/3/07). */
/*********************************************************/

/*
-----------------------------
   Global Constants
-----------------------------
*/
/*** Updated ***/
#define   CODEVERSION        20012                                             //(2.00.12 - LR)
#define   MAGICNUMBER        516114521
#define   VERSION            200
#define   EOFMARK            0x1A  /* Use 0x04 for UNIX systems */
#define   TZERO     1.E-4    /* Zero time tolerance                    */
#define   TRUE      1
#define   FALSE     0
#define   FULL      2
#define   BIG       1.E10
#define   TINY      1.E-6
#define   MISSING   -1.E10
#define   PI        3.141592654

/*** Updated 9/7/00 ***/
/* Various conversion factors */
#define   GPMperCFS   448.831 
#define   AFDperCFS   1.9837
#define   MGDperCFS   0.64632
#define   IMGDperCFS  0.5382
#define   LPSperCFS   28.317
#define   LPMperCFS   1699.0
#define   CMHperCFS   101.94
#define   CMDperCFS   2446.6
#define   MLDperCFS   2.4466
#define   M3perFT3    0.028317
#define   LperFT3     28.317
#define   MperFT      0.3048
#define   PSIperFT    0.4333
#define   KPAperPSI   6.895
#define   KWperHP     0.7457
#define   SECperDAY   86400

#define   DIFFUS    1.3E-8   /* Diffusivity of chlorine                */
							 /* @ 20 deg C (sq ft/sec)                 */
#define   VISCOS    1.1E-5   /* Kinematic viscosity of water           */
							 /* @ 20 deg C (sq ft/sec)                 */

#define   SEPSTR    " \t\n\r"  /* Token separator characters */

/*
---------------------------------------------------------------------
   Macro to test for successful allocation of memory
---------------------------------------------------------------------
*/
#define  MEMCHECK(x)  (((x) == NULL) ? 101 : 0 )
#define  FREE(x)      (free((x)))

/*
---------------------------------------------------------------------
   Conversion macros to be used in place of functions
---------------------------------------------------------------------
*/

//#include "text.h"

	//#define INT(x)   ((int)(x))                   /* integer portion of x  */
#define FRAC(x)  ((x)-(int)(x))               /* fractional part of x  */
#define ABS(x)   (((x)<0) ? -(x) : (x))       /* absolute value of x   */
#define MIN(x,y) (((x)<=(y)) ? (x) : (y))     /* minimum of x and y    */
#define MAX(x,y) (((x)>=(y)) ? (x) : (y))     /* maximum of x and y    */
#define ROUND(x) (((x)>=0) ? (int)((x)+.5) : (int)((x)-.5))
	/* round-off of x        */
#define MOD(x,y) ((x)%(y))                    /* x modulus y           */
#define SQR(x)   ((x)*(x))                    /* x-squared             */
#define SGN(x)   (((x)<0) ? (-1) : (1))       /* sign of x             */
#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))
											  /* uppercase char of x   */
/*
------------------------------------------------------
   Macro to evaluate function x with error checking
   (Fatal errors are numbered higher than 100)
------------------------------------------------------
*/
#define ERRCODE(x) (errcode = ((errcode>100) ? (errcode) : (x))) 

/*
------------------------------------------------------
   Macro to find Pump index of Link[x]
   (Diameter = pump index for pump links)
------------------------------------------------------
*/
#define PUMPINDEX(x) (ROUND(Link[(x)].Diam))

	/*hydraul*/
#define   QZERO  1.e-6  /* Equivalent to zero flow */
#define   CBIG   1.e8   /* Big coefficient         */
#define   CSMALL 1.e-6  /* Small coefficient       */

	/* Constants used for computing Darcy-Weisbach friction factor */
#define A1  0.314159265359e04  /* 1000*PI */
#define A2  0.157079632679e04  /* 500*PI  */
#define A3  0.502654824574e02  /* 16*PI   */
#define A4  6.283185307        /* 2*PI    */
#define A8  4.61841319859      /* 5.74*(PI/4)^.9 */
#define A9  -8.685889638e-01   /* -2/ln(10)      */
#define AA  -1.5634601348      /* -2*.9*2/ln(10) */
#define AB  3.28895476345e-03  /* 5.74/(4000^.9) */
#define AC  -5.14214965799e-03 /* AA*AB */

	/*quality*/
#define   UP_NODE(x)   ( (FlowDir[(x)]=='+') ? Link[(x)].N1 : Link[(x)].N2 )
#define   DOWN_NODE(x) ( (FlowDir[(x)]=='+') ? Link[(x)].N2 : Link[(x)].N1 )
#define   LINKVOL(k)   ( 0.785398*Link[(k)].Len*SQR(Link[(k)].Diam) )

	/*output*/
#define   FSAVE(n)  (fwrite(x+1,sizeof(REAL4),(n),OutFile))

/*
--------------------- Module Global Variables  ----------------------
*/

#define   MAXITER   200      /* Default max. # hydraulic iterations    */      //(2.00.12 - LR)
#define   HACC      0.001    /* Default hydraulics convergence ratio   */
#define   HTOL      0.0005   /* Default hydraulic head tolerance (ft)  */

/*** Updated 11/19/01 ***/
#define   QTOL      0.0001   /* Default flow rate tolerance (cfs)      */

#define   AGETOL    0.01     /* Default water age tolerance (hrs)      */
#define   CHEMTOL   0.01     /* Default concentration tolerance        */
#define   PAGESIZE  0        /* Default uses no page breaks            */
#define   SPGRAV    1.0      /* Default specific gravity               */
#define   EPUMP     75       /* Default pump efficiency                */
#define   DEFPATID  "1"      /* Default demand pattern ID              */

/*
These next three parameters are used in the hydraulics solver:
*/

#define   RQTOL     1E-7     /* Default low flow resistance tolerance  */
#define   CHECKFREQ 2        /* Default status check frequency         */
#define   MAXCHECK  10       /* Default # iterations for status checks */
#define   DAMPLIMIT 0        /* Default damping threshold              */      //(2.00.12 - LR)

namespace epanet {

	void (*viewprog) (char *);     /* Pointer to progress viewing function */

	/*            EPANET function           */

	int EPANET::ENepanet(char *f1, char *f2, char *f3, void(*pviewprog) (char *))

		/*------------------------------------------------------------------------
		**   Input:   f1 = pointer to name of input file
		**            f2 = pointer to name of report file
		**            f3 = pointer to name of binary output file
		**            pviewprog = see note below
		**   Output:  none
		**  Returns: error code
		**  Purpose: runs a complete EPANET simulation
		**
		**  The pviewprog() argument is a pointer to a callback function
		**  that takes a character string (char *) as its only parameter.
		**  The function would reside in and be used by the calling
		**  program to display the progress messages that EPANET generates
		**  as it carries out its computations. If this feature is not
		**  needed then the argument should be NULL.
		**-------------------------------------------------------------------------
		*/
	{
		int  errcode = 0;
		viewprog = pviewprog;
		ERRCODE(ENopen(f1, f2, f3));
		if (Hydflag != USE) ERRCODE(ENsolveH());
		ERRCODE(ENsolveQ());
		ERRCODE(ENreport());
		ENclose();
		return(errcode);
	}

	int EPANET::ENopen(const char *f1, const char *f2,const  char *f3)  // add const by zhouli
	/*----------------------------------------------------------------
	**  Input:   f1 = pointer to name of input file
	**           f2 = pointer to name of report file
	**           f3 = pointer to name of binary output file
	**  Output:  none
	**  Returns: error code
	**  Purpose: opens EPANET input file & reads in network data
	**----------------------------------------------------------------
	*/
	{
		int  errcode = 0;

		/*** Updated 9/7/00 ***/
		/* Reset math coprocessor */
		//#ifdef DLL
		//   _fpreset();              
		//#endif

		/* Set system flags */
		Openflag = FALSE;
		OpenHflag = FALSE;
		OpenQflag = FALSE;
		SaveHflag = FALSE;
		SaveQflag = FALSE;
		Warnflag = FALSE;

		/*** Updated 9/7/00 ***/
		Messageflag = TRUE;

		/* If binary output file being used, then   */
		/* do not write full results to Report file */
		/* (use it only for status reports).        */
		Rptflag = 0;
		if (strlen(f3) == 0) Rptflag = 1;

		/*** Updated 9/7/00 ***/
		/*** Previous code segment ignored. ***/
		/*** Rptflag now always set to 1.   ***/
		Rptflag = 1;

		/* Initialize global pointers to NULL. */
		initpointers();

		/* Open input & report files */
		ERRCODE(openfiles(f1, f2, f3));
		if (errcode > 0)
		{
			errmsg(errcode);
			return(errcode);
		}
		writelogo();

		/* Find network size & allocate memory for data */
		writecon(FMT02);
		writewin(FMT100);
		ERRCODE(netsize());
		ERRCODE(allocdata());

		/* Retrieve input data */
		ERRCODE(getdata());

		/* Free temporary linked lists used for Patterns & Curves */
		freeTmplist(Patlist);
		freeTmplist(Curvelist);

		/* If using previously saved hydraulics then open its file */
		if (Hydflag == USE) ERRCODE(openhydfile());

		/* Write input summary to report file */
		if (!errcode)
		{
			if (Summaryflag) writesummary();
			writetime(FMT104);
			Openflag = TRUE;
		}
		else errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENsaveinpfile(char *filename)
		/*----------------------------------------------------------------
		**  Input:   filename = name of INP file
		**  Output:  none
		**  Returns: error code
		**  Purpose: saves current data base to file
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		return(saveinpfile(filename));
	}


	int EPANET::ENclose()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: frees all memory & files used by EPANET
		**----------------------------------------------------------------
		*/
	{
		if (Openflag) writetime(FMT105);
		freedata();

		if (TmpOutFile != OutFile)                                                  //(2.00.12 - LR)
		{                                                                           //(2.00.12 - LR)
			if (TmpOutFile != NULL) fclose(TmpOutFile);                              //(2.00.12 - LR)
			remove(TmpFname);                                                        //(2.00.12 - LR)
		}                                                                           //(2.00.12 - LR)

		if (InFile != NULL) fclose(InFile);
		if (RptFile != NULL) fclose(RptFile);
		if (HydFile != NULL) fclose(HydFile);
		if (OutFile != NULL) fclose(OutFile);

		if (Hydflag == SCRATCH) remove(HydFname);                                   //(2.00.12 - LR)
		if (Outflag == SCRATCH) remove(OutFname);                                   //(2.00.12 - LR)

		Openflag = FALSE;
		OpenHflag = FALSE;
		SaveHflag = FALSE;
		OpenQflag = FALSE;
		SaveQflag = FALSE;
		return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for running a hydraulic analysis
	----------------------------------------------------------------
	*/


	int EPANET::ENsolveH()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: solves for network hydraulics in all time periods
		**----------------------------------------------------------------
		*/
	{
		int  errcode;
		long t, tstep;

		/* Open hydraulics solver */
		errcode = ENopenH();
		if (!errcode)
		{
			/* Initialize hydraulics */
			errcode = ENinitH(EN_SAVE);
			writecon(FMT14);

			/* Analyze each hydraulic period */
			if (!errcode) do
			{

				/* Display progress message */

		  /*** Updated 6/24/02 ***/
				sprintf(Msg, "%-10s", clocktime(Atime, Htime));

				writecon(Msg);
				sprintf(Msg, FMT101, Atime);
				writewin(Msg);

				/* Solve for hydraulics & advance to next time period */
				tstep = 0;
				ERRCODE(ENrunH(&t));
				ERRCODE(ENnextH(&tstep));

				/*** Updated 6/24/02 ***/
				writecon("\b\b\b\b\b\b\b\b\b\b");
			} while (tstep > 0);
		}

		/* Close hydraulics solver */

		/*** Updated 6/24/02 ***/
		writecon("\b\b\b\b\b\b\b\b                     ");

		ENcloseH();
		errcode = MAX(errcode, Warnflag);
		return(errcode);
	}


	int EPANET::ENsaveH()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: saves hydraulic results to binary file.
		**
		**  Must be called before ENreport() if no WQ simulation made.
		**  Should not be called if ENsolveQ() will be used.
		**----------------------------------------------------------------
		*/
	{
		char tmpflag;
		int  errcode;

		/* Check if hydraulic results exist */
		if (!SaveHflag) return(104);

		/* Temporarily turn off WQ analysis */
		tmpflag = Qualflag;
		Qualflag = NONE;

		/* Call WQ solver to simply transfer results */
		/* from Hydraulics file to Output file at    */
		/* fixed length reporting time intervals.    */
		errcode = ENsolveQ();

		/* Restore WQ analysis option */
		Qualflag = tmpflag;
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENopenH()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets up data structures for hydraulic analysis
		**----------------------------------------------------------------
		*/
	{
		int  errcode = 0;

		/* Check that input data exists */
		OpenHflag = FALSE;
		SaveHflag = FALSE;
		if (!Openflag) return(102);

		/* Check that previously saved hydraulics file not in use */
		if (Hydflag == USE) return(107);

		/* Open hydraulics solver */
		ERRCODE(openhyd());
		if (!errcode) OpenHflag = TRUE;
		else errmsg(errcode);
		return(errcode);
	}


	/*** Updated 3/1/01 ***/
	int EPANET::ENinitH(int flag)
		/*----------------------------------------------------------------
		**  Input:   flag = 2-digit flag where 1st (left) digit indicates
		**                  if link flows should be re-initialized (1) or
		**                  not (0) and 2nd digit indicates if hydraulic
		**                  results should be saved to file (1) or not (0)
		**  Output:  none
		**  Returns: error code
		**  Purpose: initializes hydraulic analysis
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;
		int sflag, fflag;

		/* Reset status flags */
		SaveHflag = FALSE;
		Warnflag = FALSE;

		/* Get values of save-to-file flag and reinitialize-flows flag */
		fflag = flag / EN_INITFLOW;
		sflag = flag - fflag * EN_INITFLOW;

		/* Check that hydraulics solver was opened */
		if (!OpenHflag) return(103);

		/* Open hydraulics file */
		Saveflag = FALSE;
		if (sflag > 0)
		{
			errcode = openhydfile();
			if (!errcode) Saveflag = TRUE;
			else errmsg(errcode);
		}

		/* Initialize hydraulics */
		inithyd(fflag);
		if (Statflag > 0) writeheader(STATHDR, 0);
		return(errcode);
	}


	int EPANET::ENrunH(long *t)
		/*----------------------------------------------------------------
		**  Input:   none (no need to supply a value for *t)
		**  Output:  *t = current simulation time (seconds)
		**  Returns: error/warning code
		**  Purpose: solves hydraulics for conditions at time t.
		**
		**  This function is used in a loop with ENnextH() to run
		**  an extended period hydraulic simulation.
		**  See ENsolveH() for an example.
		**----------------------------------------------------------------
		*/
	{
		int errcode;
		*t = 0;
		if (!OpenHflag) return(103);
		errcode = runhyd(t);
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENnextH(long *tstep)
		/*----------------------------------------------------------------
		**  Input:   none (no need to supply a value for *tstep)
		**  Output:  *tstep = time (seconds) until next hydraulic event
		**                    (0 marks end of simulation period)
		**  Returns: error code
		**  Purpose: determines time until next hydraulic event.
		**
		**  This function is used in a loop with ENrunH() to run
		**  an extended period hydraulic simulation.
		**  See ENsolveH() for an example.
		**----------------------------------------------------------------
		*/
	{
		int errcode;
		*tstep = 0;
		if (!OpenHflag) return(103);
		errcode = nexthyd(tstep);
		if (errcode) errmsg(errcode);
		else if (Saveflag && *tstep == 0) SaveHflag = TRUE;
		return(errcode);
	}


	int EPANET::ENcloseH()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: frees data allocated by hydraulics solver
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		closehyd();
		OpenHflag = FALSE;
		return(0);
	}


	int EPANET::ENsavehydfile(char *filename)
		/*----------------------------------------------------------------
		**  Input:   filename = name of file
		**  Output:  none
		**  Returns: error code
		**  Purpose: copies binary hydraulics file to disk
		**----------------------------------------------------------------
		*/
	{
		FILE *f;
		int   c;

		/* Check that hydraulics results exist */
		if (HydFile == NULL || !SaveHflag) return(104);

		/* Open file */
		if ((f = fopen(filename, "w+b")) == NULL) return(305);

		/* Copy from HydFile to f */
		fseek(HydFile, 0, SEEK_SET);
		while ((c = fgetc(HydFile)) != EOF) fputc(c, f);
		fclose(f);
		return(0);
	}


	int EPANET::ENusehydfile(char *filename)
		/*----------------------------------------------------------------
		**  Input:   filename = name of file
		**  Output:  none
		**  Returns: error code
		**  Purpose: opens previously saved binary hydraulics file
		**----------------------------------------------------------------
		*/
	{
		int errcode;

		/* Check that input data exists & hydraulics system closed */
		if (!Openflag) return(102);
		if (OpenHflag) return(108);

		/* Try to open hydraulics file */
		strncpy(HydFname, filename, MAXFNAME);
		Hydflag = USE;
		SaveHflag = TRUE;
		errcode = openhydfile();

		/* If error, then reset flags */
		if (errcode)
		{
			strcpy(HydFname, "");
			Hydflag = SCRATCH;
			SaveHflag = FALSE;
		}
		return(errcode);
	}


	/*
	----------------------------------------------------------------
	   Functions for running a WQ analysis
	----------------------------------------------------------------
	*/


	int EPANET::ENsolveQ()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: solves for network water quality in all time periods
		**----------------------------------------------------------------
		*/
	{
		int  errcode;
		long t, tstep;

		/* Open WQ solver */
		errcode = ENopenQ();
		if (!errcode)
		{
			/* Initialize WQ */
			errcode = ENinitQ(EN_SAVE);
			if (Qualflag) writecon(FMT15);
			else
			{
				writecon(FMT16);
				writewin(FMT103);
			}

			/* Analyze each hydraulic period */
			if (!errcode) do
			{

				/* Display progress message */

		  /*** Updated 6/24/02 ***/
				sprintf(Msg, "%-10s", clocktime(Atime, Htime));

				writecon(Msg);
				if (Qualflag)
				{
					sprintf(Msg, FMT102, Atime);
					writewin(Msg);
				}

				/* Retrieve current network solution & update WQ to next time period */
				tstep = 0;
				ERRCODE(ENrunQ(&t));
				ERRCODE(ENnextQ(&tstep));

				/*** Updated 6/24/02 ***/
				writecon("\b\b\b\b\b\b\b\b\b\b");

			} while (tstep > 0);

		}

		/* Close WQ solver */

		/*** Updated 6/24/02 ***/
		writecon("\b\b\b\b\b\b\b\b                     ");
		ENcloseQ();
		return(errcode);
	}


	int EPANET::ENopenQ()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets up data structures for WQ analysis
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;

		/* Check that hydraulics results exist */
		OpenQflag = FALSE;
		SaveQflag = FALSE;
		if (!Openflag) return(102);
		if (!SaveHflag) return(104);

		/* Open WQ solver */
		ERRCODE(openqual());
		if (!errcode) OpenQflag = TRUE;
		else errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENinitQ(int saveflag)
		/*----------------------------------------------------------------
		**  Input:   saveflag = EN_SAVE (1) if results saved to file,
		**                      EN_NOSAVE (0) if not
		**  Output:  none
		**  Returns: error code
		**  Purpose: initializes WQ analysis
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;
		if (!OpenQflag) return(105);
		initqual();
		SaveQflag = FALSE;
		Saveflag = FALSE;
		if (saveflag)
		{
			errcode = openoutfile();
			if (!errcode) Saveflag = TRUE;
		}
		return(errcode);
	}


	int EPANET::ENrunQ(long *t)
		/*----------------------------------------------------------------
		**  Input:   none (no need to supply a value for *t)
		**  Output:  *t = current simulation time (seconds)
		**  Returns: error code
		**  Purpose: retrieves hydraulic & WQ results at time t.
		**
		**  This function is used in a loop with ENnextQ() to run
		**  an extended period WQ simulation. See ENsolveQ() for
		**  an example.
		**----------------------------------------------------------------
		*/
	{
		int errcode;
		*t = 0;
		if (!OpenQflag) return(105);
		errcode = runqual(t);
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENnextQ(long *tstep)
		/*----------------------------------------------------------------
		**  Input:   none (no need to supply a value for *tstep)
		**  Output:  *tstep = time (seconds) until next hydraulic event
		**                    (0 marks end of simulation period)
		**  Returns: error code
		**  Purpose: advances WQ simulation to next hydraulic event.
		**
		**  This function is used in a loop with ENrunQ() to run
		**  an extended period WQ simulation. See ENsolveQ() for
		**  an example.
		**----------------------------------------------------------------
		*/
	{
		int errcode;
		*tstep = 0;
		if (!OpenQflag) return(105);
		errcode = nextqual(tstep);
		if (!errcode && Saveflag && *tstep == 0) SaveQflag = TRUE;
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENstepQ(long *tleft)
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  *tleft = time left in overall simulation (seconds)
		**  Returns: error code
		**  Purpose: advances WQ simulation by a single WQ time Step
		**
		**  This function is used in a loop with ENrunQ() to run
		**  an extended period WQ simulation.
		**----------------------------------------------------------------
		*/
	{
		int errcode;
		*tleft = 0;
		if (!OpenQflag) return(105);
		errcode = stepqual(tleft);
		if (!errcode && Saveflag && *tleft == 0) SaveQflag = TRUE;
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int EPANET::ENcloseQ()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: frees data allocated by WQ solver
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		closequal();
		OpenQflag = FALSE;
		return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for generating an output report
	----------------------------------------------------------------
	*/


	int EPANET::ENwriteline(char *line)
		/*----------------------------------------------------------------
		**  Input:   line = text string
		**  Output:  none
		**  Returns: error code
		**  Purpose: writes line of text to report file
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		writeline(line);
		return(0);
	}


	int EPANET::ENreport()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: writes report to report file
		**----------------------------------------------------------------
		*/
	{
		int  errcode;

		/* Check if results saved to binary output file */
		if (!SaveQflag) return(106);
		errcode = writereport();
		if (errcode) errmsg(errcode);
		return(errcode);
	}


	int  EPANET::ENresetreport()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: resets report options to default values
		**----------------------------------------------------------------
		*/
	{
		int i;
		if (!Openflag) return(102);
		initreport();
		for (i = 1; i <= Nnodes; i++) Node[i].Rpt = 0;
		for (i = 1; i <= Nlinks; i++) Link[i].Rpt = 0;
		return(0);
	}


	int  EPANET::ENsetreport(char *s)
		/*----------------------------------------------------------------
		**  Input:   s = report format command
		**  Output:  none
		**  Returns: error code
		**  Purpose: processes a reporting format command
		**----------------------------------------------------------------
		*/
	{
		char s1[MAXLINE + 1];
		if (!Openflag) return(102);
		if (strlen(s) > MAXLINE) return(250);
		strcpy(s1, s);
		if (setreport(s1) > 0) return(250);
		else return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for retrieving network information
	----------------------------------------------------------------
	*/

	/*** Updated 10/25/00 ***/
	int EPANET::ENgetversion(int *v)
		/*----------------------------------------------------------------
		**  Input:    none
		**  Output:   *v = version number of the source code
		**  Returns:  error code (should always be 0)
		**  Purpose:  retrieves a number assigned to the most recent
		**            update of the source code. This number, set by the
		**            constant CODEVERSION found in TYPES.H,  began with
		**            20001 and increases by 1 with each new update.
		**----------------------------------------------------------------
		*/
	{
		*v = CODEVERSION;
		return(0);
	}


	int EPANET::ENgetcontrol(int cindex, int *ctype, int *lindex,
		float *setting, int *nindex, float *level)
		/*----------------------------------------------------------------
		**  Input:   cindex   = control index (position of control statement
		**                      in the input file, starting from 1)
		**  Output:  *ctype   = control type code (see TOOLKIT.H)
		**           *lindex  = index of controlled link
		**           *setting = control setting on link
		**           *nindex  = index of controlling node (0 for TIMER
		**                      or TIMEOFDAY control)
		**           *level   = control level (tank level, junction
		**                      pressure, or time (seconds))
		**  Returns: error code
		**  Purpose: retrieves parameters that define a simple control
		**----------------------------------------------------------------
		*/
	{
		double s, lvl;

		s = 0.0;
		lvl = 0.0;
		*ctype = 0;
		*lindex = 0;
		*nindex = 0;
		if (!Openflag) return(102);
		if (cindex < 1 || cindex > Ncontrols) return(241);
		*ctype = Control[cindex].Type;
		*lindex = Control[cindex].Link;
		s = Control[cindex].Setting;
		if (Control[cindex].Setting != MISSING) switch (Link[*lindex].Type)
		{
		case PRV:
		case PSV:
		case PBV: s *= Ucf[PRESSURE]; break;
		case FCV: s *= Ucf[FLOW];
		}
		else if (Control[cindex].Status == OPEN) s = 1.0;

		/*** Updated 3/1/01 ***/
		else s = 0.0;

		*nindex = Control[cindex].Node;
		if (*nindex > Njuncs)
			lvl = (Control[cindex].Grade - Node[*nindex].El) * Ucf[ELEV];
		else if (*nindex > 0)
			lvl = (Control[cindex].Grade - Node[*nindex].El) * Ucf[PRESSURE];
		else
			lvl = (float)Control[cindex].Time;
		*setting = (float)s;
		*level = (float)lvl;
		return(0);
	}


	int EPANET::ENgetcount(int code, int *count)
		/*----------------------------------------------------------------
		**  Input:   code = component code (see TOOLKIT.H)
		**  Output:  *count = number of components in network
		**  Returns: error code
		**  Purpose: retrieves the number of components of a
		**           given type in the network
		**----------------------------------------------------------------
		*/
	{
		*count = 0;
		if (!Openflag) return(102);
		switch (code)
		{
		case EN_NODECOUNT:    *count = Nnodes;    break;
		case EN_TANKCOUNT:    *count = Ntanks;    break;
		case EN_LINKCOUNT:    *count = Nlinks;    break;
		case EN_PATCOUNT:     *count = Npats;     break;
		case EN_CURVECOUNT:   *count = Ncurves;   break;
		case EN_CONTROLCOUNT: *count = Ncontrols; break;
		default: return(251);
		}
		return(0);
	}


	int  EPANET::ENgetoption(int code, float *value)
		/*----------------------------------------------------------------
		**  Input:   code = option code (see TOOLKIT.H)
		**  Output:  *value = option value
		**  Returns: error code
		**  Purpose: gets value for an analysis option
		**----------------------------------------------------------------
		*/
	{
		double v = 0.0;
		*value = 0.0f;
		if (!Openflag) return(102);
		switch (code)
		{
		case EN_TRIALS:     v = (double)MaxIter;
			break;
		case EN_ACCURACY:   v = Hacc;
			break;
		case EN_TOLERANCE:  v = Ctol * Ucf[QUALITY];
			break;
		case EN_EMITEXPON:  if (Qexp > 0.0) v = 1.0 / Qexp;
			break;
		case EN_DEMANDMULT: v = Dmult;
			break;
		default:            return(251);
		}
		*value = (float)v;
		return(0);
	}


	int EPANET::ENgettimeparam(int code, long *value)
		/*----------------------------------------------------------------
		**  Input:   code = time parameter code (see TOOLKIT.H)
		**  Output:  *value = value of time parameter
		**  Returns: error code
		**  Purpose: retrieves value of specific time parameter
		**----------------------------------------------------------------
		*/
	{
		*value = 0;
		if (!Openflag) return(102);
		if (code < EN_DURATION || code > EN_PERIODS) return(251);
		switch (code)
		{
		case EN_DURATION:     *value = Dur;       break;
		case EN_HYDSTEP:      *value = Hstep;     break;
		case EN_QUALSTEP:     *value = Qstep;     break;
		case EN_PATTERNSTEP:  *value = Pstep;     break;
		case EN_PATTERNSTART: *value = Pstart;    break;
		case EN_REPORTSTEP:   *value = Rstep;     break;
		case EN_REPORTSTART:  *value = Rstart;    break;
		case EN_STATISTIC:    *value = Tstatflag; break;
		case EN_PERIODS:      *value = Nperiods;  break;
		}
		return(0);
	}


	int EPANET::ENgetflowunits(int *code)
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  *code = code of flow units in use
		**                   (see TOOLKIT.H or TYPES.H)
		**  Returns: error code
		**  Purpose: retrieves flow units code
		**----------------------------------------------------------------
		*/
	{
		*code = -1;
		if (!Openflag) return(102);
		*code = Flowflag;
		return(0);
	}


	int  EPANET::ENgetpatternindex(char *id, int *index)
		/*----------------------------------------------------------------
		**  Input:   id     = time pattern ID
		**  Output:  *index = index of time pattern in list of patterns
		**  Returns: error code
		**  Purpose: retrieves index of time pattern with specific ID
		**----------------------------------------------------------------
		*/
	{
		int i;
		*index = 0;
		if (!Openflag) return(102);
		for (i = 1; i <= Npats; i++)
		{
			if (strcmp(id, Pattern[i].ID) == 0)
			{
				*index = i;
				return(0);
			}
		}
		*index = 0;
		return(205);
	}


	int EPANET::ENgetpatternid(int index, char *id)
		/*----------------------------------------------------------------
		**  Input:   index = index of time pattern
		**  Output:  id    = pattern ID
		**  Returns: error code
		**  Purpose: retrieves ID of a time pattern with specific index
		**
		**  NOTE: 'id' must be able to hold MAXID characters
		**----------------------------------------------------------------
		*/
	{
		strcpy(id, "");
		if (!Openflag) return(102);
		if (index < 1 || index > Npats) return(205);
		strcpy(id, Pattern[index].ID);
		return(0);
	}


	int EPANET::ENgetpatternlen(int index, int *len)
		/*----------------------------------------------------------------
		**  Input:   index = index of time pattern
		**  Output:  *len  = pattern length (number of multipliers)
		**  Returns: error code
		**  Purpose: retrieves number of multipliers in a time pattern
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		if (index < 1 || index > Npats) return(205);
		*len = Pattern[index].Length;
		return(0);
	}


	int EPANET::ENgetpatternvalue(int index, int period, float *value)
		/*----------------------------------------------------------------
		**  Input:   index  = index of time pattern
		**           period = pattern time period
		**  Output:  *value = pattern multiplier
		**  Returns: error code
		**  Purpose: retrieves multiplier for a specific time period
		**           and pattern
		**----------------------------------------------------------------
		*/
	{
		*value = 0.0f;
		if (!Openflag) return(102);
		if (index < 1 || index > Npats) return(205);
		if (period < 1 || period > Pattern[index].Length) return(251);
		*value = (float)Pattern[index].F[period - 1];
		return(0);
	}


	int  EPANET::ENgetqualtype(int *qualcode, int *tracenode)
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  *qualcode  = WQ analysis code number (see TOOLKIT.H)
		**           *tracenode = index of node being traced (if
		**                        qualocode = WQ tracing)
		**  Returns: error code
		**  Purpose: retrieves type of quality analysis called for
		**----------------------------------------------------------------
		*/
	{
		*tracenode = 0;
		if (!Openflag) return(102);
		*qualcode = Qualflag;
		if (Qualflag == TRACE) *tracenode = TraceNode;
		return(0);
	}


	int  EPANET::ENgeterror(int errcode, char *errmsg, int n)
		/*----------------------------------------------------------------
		**  Input:   errcode = error/warning code number
		**           n       = maximum length of string errmsg
		**  Output:  errmsg  = text of error/warning message
		**  Returns: error code
		**  Purpose: retrieves text of error/warning message
		**----------------------------------------------------------------
		*/
	{
		switch (errcode)
		{
		case 1:  strncpy(errmsg, WARN1, n);   break;
		case 2:  strncpy(errmsg, WARN2, n);   break;
		case 3:  strncpy(errmsg, WARN3, n);   break;
		case 4:  strncpy(errmsg, WARN4, n);   break;
		case 5:  strncpy(errmsg, WARN5, n);   break;
		case 6:  strncpy(errmsg, WARN6, n);   break;
		default: strncpy(errmsg, geterrmsg(errcode), n);
		}
		if (strlen(errmsg) == 0) return(251);
		else return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for retrieving node data
	----------------------------------------------------------------
	*/


	int EPANET::ENgetnodeindex(char *id, int *index)
		/*----------------------------------------------------------------
		**  Input:   id = node ID
		**  Output:  *index = index of node in list of nodes
		**  Returns: error code
		**  Purpose: retrieves index of a node with specific ID
		**----------------------------------------------------------------
		*/
	{
		*index = 0;
		if (!Openflag) return(102);
		*index = findnode(id);
		if (*index == 0) return(203);
		else return(0);
	}


	int EPANET::ENgetnodeid(int index, char *id)
		/*----------------------------------------------------------------
		**  Input:   index = index of node in list of nodes
		**  Output:  id = node ID
		**  Returns: error code
		**  Purpose: retrieves ID of a node with specific index
		**
		**  NOTE: 'id' must be able to hold MAXID characters
		**----------------------------------------------------------------
		*/
	{
		strcpy(id, "");
		if (!Openflag) return(102);
		if (index < 1 || index > Nnodes) return(203);
		strcpy(id, Node[index].ID);
		return(0);
	}


	int  EPANET::ENgetnodetype(int index, int *code)
		/*----------------------------------------------------------------
		**  Input:   index = node index
		**  Output:  *code = node type code number (see TOOLKIT.H)
		**  Returns: error code
		**  Purpose: retrieves node type of specific node
		**----------------------------------------------------------------
		*/
	{
		*code = -1;
		if (!Openflag) return(102);
		if (index < 1 || index > Nnodes) return(203);
		if (index <= Njuncs) *code = EN_JUNCTION;
		else
		{
			if (Tank[index - Njuncs].A == 0.0) *code = EN_RESERVOIR;
			else *code = EN_TANK;
		}
		return(0);
	}


	int EPANET::ENgetnodevalue(int index, int code, float *value)
		/*----------------------------------------------------------------
		**  Input:   index = node index
		**           code  = node parameter code (see TOOLKIT.H)
		**  Output:  *value = value of node's parameter
		**  Returns: error code
		**  Purpose: retrieves parameter value for a node
		**----------------------------------------------------------------
		*/
	{
		double v = 0.0;
		Pdemand demand;
		Psource source;

		/* Check for valid arguments */
		*value = 0.0f;
		if (!Openflag) return(102);
		if (index <= 0 || index > Nnodes) return(203);

		/* Retrieve called-for parameter */
		switch (code)
		{
		case EN_ELEVATION:
			v = Node[index].El * Ucf[ELEV];
			break;

		case EN_BASEDEMAND:
			v = 0.0;
			/* NOTE: primary demand category is last on demand list */
			if (index <= Njuncs)
				for (demand = Node[index].D; demand != NULL; demand = demand->next)
					v = (demand->Base);
			v *= Ucf[FLOW];
			break;

		case EN_PATTERN:
			v = 0.0;
			/* NOTE: primary demand category is last on demand list */
			if (index <= Njuncs)
			{
				for (demand = Node[index].D; demand != NULL; demand = demand->next)
					v = (double)(demand->Pat);
			}
			else v = (double)(Tank[index - Njuncs].Pat);
			break;

		case EN_EMITTER:
			v = 0.0;
			if (Node[index].Ke > 0.0)
				v = Ucf[FLOW] / pow((Ucf[PRESSURE] * Node[index].Ke), (1.0 / Qexp));
			break;

		case EN_INITQUAL:
			v = Node[index].C0 * Ucf[QUALITY];
			break;

			/*** Additional parameters added for retrieval ***/                            //(2.00.11 - LR)
		case EN_SOURCEQUAL:
		case EN_SOURCETYPE:
		case EN_SOURCEMASS:
		case EN_SOURCEPAT:
			source = Node[index].S;
			if (source == NULL) return(240);
			if (code == EN_SOURCEQUAL)      v = source->C0;
			else if (code == EN_SOURCEMASS) v = source->Smass * 60.0;
			else if (code == EN_SOURCEPAT)  v = source->Pat;
			else                            v = source->Type;
			return(0);

		case EN_TANKLEVEL:
			if (index <= Njuncs) return(251);
			v = (Tank[index - Njuncs].H0 - Node[index].El) * Ucf[ELEV];
			break;

			/*** New parameter added for retrieval ***/                                    //(2.00.11 - LR)
		case EN_INITVOLUME:                                                      //(2.00.11 - LR)
			v = 0.0;                                                              //(2.00.11 - LR)
			if (index > Njuncs) v = Tank[index - Njuncs].V0 * Ucf[VOLUME];          //(2.00.11 - LR)
			break;                                                                //(2.00.11 - LR)

   /*** New parameter added for retrieval ***/                                    //(2.00.11 - LR)
		case EN_MIXMODEL:                                                        //(2.00.11 - LR)
			v = MIX1;                                                             //(2.00.11 - LR)
			if (index > Njuncs) v = Tank[index - Njuncs].MixModel;                //(2.00.11 - LR)
			break;                                                                //(2.00.11 - LR)

   /*** New parameter added for retrieval ***/                                    //(2.00.11 - LR)
		case EN_MIXZONEVOL:                                                      //(2.00.11 - LR)
			v = 0.0;                                                              //(2.00.11 - LR)
			if (index > Njuncs) v = Tank[index - Njuncs].V1max * Ucf[VOLUME];       //(2.00.11 - LR)
			break;                                                                //(2.00.11 - LR)

		case EN_DEMAND:
			v = D[index] * Ucf[FLOW];
			break;

		case EN_HEAD:
			v = H[index] * Ucf[HEAD];
			break;

		case EN_PRESSURE:
			v = (H[index] - Node[index].El) * Ucf[PRESSURE];
			break;

		case EN_QUALITY:
			v = C[index] * Ucf[QUALITY];
			break;

			/*** New parameters added for retrieval begins here   ***/                     //(2.00.12 - LR)
			/*** (Thanks to Nicolas Basile of Ecole Polytechnique ***/
			/***  de Montreal for suggesting some of these.)      ***/

		case EN_TANKDIAM:
			v = 0.0;
			if (index > Njuncs)
			{
				v = 4.0 / PI * sqrt(Tank[index - Njuncs].A) * Ucf[ELEV];
			}
			break;

		case EN_MINVOLUME:
			v = 0.0;
			if (index > Njuncs) v = Tank[index - Njuncs].Vmin * Ucf[VOLUME];
			break;

		case EN_VOLCURVE:
			v = 0.0;
			if (index > Njuncs) v = Tank[index - Njuncs].Vcurve;
			break;

		case EN_MINLEVEL:
			v = 0.0;
			if (index > Njuncs)
			{
				v = (Tank[index - Njuncs].Hmin - Node[index].El) * Ucf[ELEV];
			}
			break;

		case EN_MAXLEVEL:
			v = 0.0;
			if (index > Njuncs)
			{
				v = (Tank[index - Njuncs].Hmax - Node[index].El) * Ucf[ELEV];
			}
			break;

		case EN_MIXFRACTION:
			v = 1.0;
			if (index > Njuncs && Tank[index - Njuncs].Vmax > 0.0)
			{
				v = Tank[index - Njuncs].V1max / Tank[index - Njuncs].Vmax;
			}
			break;

		case EN_TANK_KBULK:
			v = 0.0;
			if (index > Njuncs) v = Tank[index - Njuncs].Kb * SECperDAY;
			break;

			/***  New parameter additions ends here. ***/                                  //(2.00.12 - LR)

		default: return(251);
		}
		*value = (float)v;
		return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for retrieving link data
	----------------------------------------------------------------
	*/


	int EPANET::ENgetlinkindex(char *id, int *index)
		/*----------------------------------------------------------------
		**  Input:   id = link ID
		**  Output:  *index = index of link in list of links
		**  Returns: error code
		**  Purpose: retrieves index of a link with specific ID
		**----------------------------------------------------------------
		*/
	{
		*index = 0;
		if (!Openflag) return(102);
		*index = findlink(id);
		if (*index == 0) return(204);
		else return(0);
	}


	int EPANET::ENgetlinkid(int index, char *id)
		/*----------------------------------------------------------------
		**  Input:   index = index of link in list of links
		**  Output:  id = link ID
		**  Returns: error code
		**  Purpose: retrieves ID of a link with specific index
		**
		**  NOTE: 'id' must be able to hold MAXID characters
		**----------------------------------------------------------------
		*/
	{
		strcpy(id, "");
		if (!Openflag) return(102);
		if (index < 1 || index > Nlinks) return(204);
		strcpy(id, Link[index].ID);
		return(0);
	}


	int  EPANET::ENgetlinktype(int index, int *code)
		/*------------------------------------------------------------------
		**  Input:   index = link index
		**  Output:  *code = link type code number (see TOOLKIT.H)
		**  Returns: error code
		**  Purpose: retrieves link type of specific link
		**------------------------------------------------------------------
		*/
	{
		*code = -1;
		if (!Openflag) return(102);
		if (index < 1 || index > Nlinks) return(204);
		*code = Link[index].Type;
		return(0);
	}


	int  EPANET::ENgetlinknodes(int index, int *node1, int *node2)
		/*----------------------------------------------------------------
		**  Input:   index = link index
		**  Output:  *node1 = index of link's starting node
		**           *node2 = index of link's ending node
		**  Returns: error code
		**  Purpose: retrieves end nodes of a specific link
		**----------------------------------------------------------------
		*/
	{
		*node1 = 0;
		*node2 = 0;
		if (!Openflag) return(102);
		if (index < 1 || index > Nlinks) return(204);
		*node1 = Link[index].N1;
		*node2 = Link[index].N2;
		return(0);
	}


	int EPANET::ENgetlinkvalue(int index, int code, float *value)
		/*------------------------------------------------------------------
		**  Input:   index = link index
		**           code  = link parameter code (see TOOLKIT.H)
		**  Output:  *value = value of link's parameter
		**  Returns: error code
		**  Purpose: retrieves parameter value for a link
		**------------------------------------------------------------------
		*/
	{
		double a, h, q, v = 0.0;

		/* Check for valid arguments */
		*value = 0.0f;
		if (!Openflag) return(102);
		if (index <= 0 || index > Nlinks) return(204);

		/* Retrieve called-for parameter */
		switch (code)
		{
		case EN_DIAMETER:
			if (Link[index].Type == PUMP) v = 0.0;
			else v = Link[index].Diam * Ucf[DIAM];
			break;

		case EN_LENGTH:
			v = Link[index].Len * Ucf[ELEV];
			break;

		case EN_ROUGHNESS:
			if (Link[index].Type <= PIPE)
			{
				if (Formflag == DW)
					v = Link[index].Kc * (1000.0 * Ucf[ELEV]);
				else v = Link[index].Kc;
			}
			else v = 0.0;
			break;

		case EN_MINORLOSS:
			if (Link[index].Type != PUMP)
			{
				v = Link[index].Km;
				v *= (SQR(Link[index].Diam) * SQR(Link[index].Diam) / 0.02517);
			}
			else v = 0.0;
			break;

		case EN_INITSTATUS:
			if (Link[index].Stat <= CLOSED) v = 0.0;
			else v = 1.0;
			break;

		case EN_INITSETTING:
			if (Link[index].Type == PIPE || Link[index].Type == CV)
				return(ENgetlinkvalue(index, EN_ROUGHNESS, value));
			v = Link[index].Kc;
			switch (Link[index].Type)
			{
			case PRV:
			case PSV:
			case PBV: v *= Ucf[PRESSURE]; break;
			case FCV: v *= Ucf[FLOW];
			}
			break;

		case EN_KBULK:
			v = Link[index].Kb * SECperDAY;
			break;

		case EN_KWALL:
			v = Link[index].Kw * SECperDAY;
			break;

		case EN_FLOW:

			/*** Updated 10/25/00 ***/
			if (S[index] <= CLOSED) v = 0.0;

			else v = Q[index] * Ucf[FLOW];
			break;

		case EN_VELOCITY:
			if (Link[index].Type == PUMP) v = 0.0;

			/*** Updated 11/19/01 ***/
			else if (S[index] <= CLOSED) v = 0.0;

			else
			{
				q = ABS(Q[index]);
				a = PI * SQR(Link[index].Diam) / 4.0;
				v = q / a * Ucf[VELOCITY];
			}
			break;

		case EN_HEADLOSS:

			/*** Updated 11/19/01 ***/
			if (S[index] <= CLOSED) v = 0.0;

			else
			{
				h = H[Link[index].N1] - H[Link[index].N2];
				if (Link[index].Type != PUMP) h = ABS(h);
				v = h * Ucf[HEADLOSS];
			}
			break;

		case EN_STATUS:
			if (S[index] <= CLOSED) v = 0.0;
			else v = 1.0;
			break;

		case EN_SETTING:
			if (Link[index].Type == PIPE || Link[index].Type == CV)
				return(ENgetlinkvalue(index, EN_ROUGHNESS, value));
			if (K[index] == MISSING) v = 0.0;
			else                     v = K[index];
			switch (Link[index].Type)
			{
			case PRV:
			case PSV:
			case PBV: v *= Ucf[PRESSURE]; break;
			case FCV: v *= Ucf[FLOW];
			}
			break;

		case EN_ENERGY:
			getenergy(index, &v, &a);
			break;

		default: return(251);
		}
		*value = (float)v;
		return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for changing network data
	----------------------------------------------------------------
	*/


	int EPANET::ENsetcontrol(int cindex, int ctype, int lindex,
		float setting, int nindex, float level)
		/*----------------------------------------------------------------
		**  Input:   cindex  = control index (position of control statement
		**                     in the input file, starting from 1)
		**           ctype   = control type code (see TOOLKIT.H)
		**           lindex  = index of controlled link
		**           setting = control setting applied to link
		**           nindex  = index of controlling node (0 for TIMER
		**                     or TIMEOFDAY control)
		**           level   = control level (tank level, junction pressure,
		**                     or time (seconds))
		**  Output:  none
		**  Returns: error code
		**  Purpose: specifies parameters that define a simple control
		**----------------------------------------------------------------
		*/
	{
		char   status = ACTIVE;
		long   t = 0;
		double s = setting, lvl = level;

		/* Check that input file opened */
		if (!Openflag) return(102);

		/* Check that control exists */
		if (cindex < 1 || cindex > Ncontrols) return(241);

		/* Check that controlled link exists */
		if (lindex == 0)
		{
			Control[cindex].Link = 0;
			return(0);
		}
		if (lindex < 0 || lindex > Nlinks) return(204);

		/* Cannot control check valve. */
		if (Link[lindex].Type == CV) return(207);

		/* Check for valid parameters */
		if (ctype < 0 || ctype > EN_TIMEOFDAY) return(251);
		if (ctype == EN_LOWLEVEL || ctype == EN_HILEVEL)
		{
			if (nindex < 1 || nindex > Nnodes) return(203);
		}
		else nindex = 0;
		if (s < 0.0 || lvl < 0.0) return(202);

		/* Adjust units of control parameters */
		switch (Link[lindex].Type)
		{
		case PRV:
		case PSV:
		case PBV:  s /= Ucf[PRESSURE];
			break;
		case FCV:  s /= Ucf[FLOW];
			break;

			/*** Updated 9/7/00 ***/
		case GPV:  if (s == 0.0) status = CLOSED;
				else if (s == 1.0) status = OPEN;
				else return(202);
			s = Link[lindex].Kc;
			break;

		case PIPE:
		case PUMP: status = OPEN;
			if (s == 0.0) status = CLOSED;
		}
		if (ctype == LOWLEVEL || ctype == HILEVEL)
		{
			if (nindex > Njuncs) lvl = Node[nindex].El + level / Ucf[ELEV];
			else lvl = Node[nindex].El + level / Ucf[PRESSURE];
		}
		if (ctype == TIMER)     t = (long)ROUND(lvl);
		if (ctype == TIMEOFDAY) t = (long)ROUND(lvl) % SECperDAY;

		/* Reset control's parameters */
		Control[cindex].Type = (char)ctype;
		Control[cindex].Link = lindex;
		Control[cindex].Node = nindex;
		Control[cindex].Status = status;
		Control[cindex].Setting = s;
		Control[cindex].Grade = lvl;
		Control[cindex].Time = t;
		return(0);
	}


	int EPANET::ENsetnodevalue(int index, int code, float v)
		/*----------------------------------------------------------------
		**  Input:   index = node index
		**           code  = node parameter code (see TOOLKIT.H)
		**           value = parameter value
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets input parameter value for a node
		**----------------------------------------------------------------
		*/
	{
		int  j;
		Pdemand demand;
		Psource source;
		double value = v;

		if (!Openflag) return(102);
		if (index <= 0 || index > Nnodes) return(203);
		switch (code)
		{
		case EN_ELEVATION:
			if (index <= Njuncs) Node[index].El = value / Ucf[ELEV];
			else
			{
				value = (value / Ucf[ELEV]) - Node[index].El;
				j = index - Njuncs;
				Tank[j].H0 += value;
				Tank[j].Hmin += value;
				Tank[j].Hmax += value;
				Node[index].El += value;
				H[index] += value;
			}
			break;

		case EN_BASEDEMAND:
			/* NOTE: primary demand category is last on demand list */
			if (index <= Njuncs)
			{
				for (demand = Node[index].D; demand != NULL; demand = demand->next)
				{
					if (demand->next == NULL) demand->Base = value / Ucf[FLOW];
				}
			}
			break;

		case EN_PATTERN:
			/* NOTE: primary demand category is last on demand list */
			j = ROUND(value);
			if (j < 0 || j > Npats) return(205);
			if (index <= Njuncs)
			{
				for (demand = Node[index].D; demand != NULL; demand = demand->next)
				{
					if (demand->next == NULL) demand->Pat = j;
				}
			}
			else Tank[index - Njuncs].Pat = j;
			break;

		case EN_EMITTER:
			if (index > Njuncs) return(203);
			if (value < 0.0) return(202);
			if (value > 0.0)
				value = pow((Ucf[FLOW] / value), Qexp) / Ucf[PRESSURE];
			Node[index].Ke = value;
			break;

		case EN_INITQUAL:
			if (value < 0.0) return(202);
			Node[index].C0 = value / Ucf[QUALITY];
			if (index > Njuncs) Tank[index - Njuncs].C = Node[index].C0;
			break;

		case EN_SOURCEQUAL:
		case EN_SOURCETYPE:
		case EN_SOURCEPAT:
			if (value < 0.0) return(202);
			source = Node[index].S;
			if (source == NULL)
			{
				source = (Ssource *)malloc(sizeof(Ssource));
				if (source == NULL) return(101);
				source->Type = CONCEN;
				source->C0 = 0.0;
				source->Pat = 0;
				Node[index].S = source;
			}
			if (code == EN_SOURCEQUAL) source->C0 = value;
			else if (code == EN_SOURCEPAT)
			{
				j = ROUND(value);
				if (j < 0 || j > Npats) return(205);
				source->Pat = j;
			}
			else
			{
				j = ROUND(value);
				if (j < CONCEN || j > FLOWPACED) return(251);
				else source->Type = (char)j;
			}
			return(0);

		case EN_TANKLEVEL:
			if (index <= Njuncs) return(251);
			j = index - Njuncs;
			if (Tank[j].A == 0.0)  /* Tank is a reservoir */
			{
				Tank[j].H0 = value / Ucf[ELEV];
				Tank[j].Hmin = Tank[j].H0;
				Tank[j].Hmax = Tank[j].H0;
				Node[index].El = Tank[j].H0;
				H[index] = Tank[j].H0;
			}
			else
			{
				value = Node[index].El + value / Ucf[ELEV];
				if (value > Tank[j].Hmax
					|| value < Tank[j].Hmin) return(202);
				Tank[j].H0 = value;
				Tank[j].V0 = tankvolume(j, Tank[j].H0);
				H[index] = Tank[j].H0;
			}
			break;

			/*** New parameters added for retrieval begins here   ***/                     //(2.00.12 - LR)
			/*** (Thanks to Nicolas Basile of Ecole Polytechnique ***/
			/***  de Montreal for suggesting some of these.)      ***/

		case EN_TANKDIAM:
			if (value <= 0.0) return(202);
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				value /= Ucf[ELEV];
				Tank[j].A = PI * SQR(value) / 4.0;
				Tank[j].Vmin = tankvolume(j, Tank[j].Hmin);
				Tank[j].V0 = tankvolume(j, Tank[j].H0);
				Tank[j].Vmax = tankvolume(j, Tank[j].Hmax);
			}
			break;

		case EN_MINVOLUME:
			if (value < 0.0) return(202);
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				Tank[j].Vmin = value / Ucf[VOLUME];
				Tank[j].V0 = tankvolume(j, Tank[j].H0);
				Tank[j].Vmax = tankvolume(j, Tank[j].Hmax);
			}
			break;

		case EN_MINLEVEL:
			if (value < 0.0) return(202);
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				if (Tank[j].Vcurve > 0) return(202);
				Tank[j].Hmin = value / Ucf[ELEV] + Node[index].El;
				Tank[j].Vmin = tankvolume(j, Tank[j].Hmin);
			}
			break;

		case EN_MAXLEVEL:
			if (value < 0.0) return(202);
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				if (Tank[j].Vcurve > 0) return(202);
				Tank[j].Hmax = value / Ucf[ELEV] + Node[index].El;
				Tank[j].Vmax = tankvolume(j, Tank[j].Hmax);
			}
			break;

		case EN_MIXMODEL:
			j = ROUND(value);
			if (j < MIX1 || j > LIFO) return(202);
			if (index > Njuncs && Tank[index - Njuncs].A > 0.0)
			{
				Tank[index - Njuncs].MixModel = (char)j;
			}
			break;

		case EN_MIXFRACTION:
			if (value < 0.0 || value > 1.0) return(202);
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				Tank[j].V1max = value * Tank[j].Vmax;
			}
			break;

		case EN_TANK_KBULK:
			j = index - Njuncs;
			if (j > 0 && Tank[j].A > 0.0)
			{
				Tank[j].Kb = value / SECperDAY;
				Reactflag = 1;
			}
			break;

			/***  New parameter additions ends here. ***/                                  //(2.00.12 - LR)

		default: return(251);
		}
		return(0);
	}


	int EPANET::ENsetlinkvalue(int index, int code, float v)
		/*----------------------------------------------------------------
		**  Input:   index = link index
		**           code  = link parameter code (see TOOLKIT.H)
		**           v = parameter value
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets input parameter value for a link
		**----------------------------------------------------------------
		*/
	{
		char  s;
		double r, value = v;

		if (!Openflag) return(102);
		if (index <= 0 || index > Nlinks) return(204);
		switch (code)
		{
		case EN_DIAMETER:
			if (Link[index].Type != PUMP)
			{
				if (value <= 0.0) return(202);
				value /= Ucf[DIAM];              /* Convert to feet */
				r = Link[index].Diam / value;      /* Ratio of old to new diam */
				Link[index].Km *= SQR(r) * SQR(r); /* Adjust minor loss factor */
				Link[index].Diam = value;        /* Update diameter */
				resistance(index);               /* Update resistance factor */
			}
			break;

		case EN_LENGTH:
			if (Link[index].Type <= PIPE)
			{
				if (value <= 0.0) return(202);
				Link[index].Len = value / Ucf[ELEV];
				resistance(index);
			}
			break;

		case EN_ROUGHNESS:
			if (Link[index].Type <= PIPE)
			{
				if (value <= 0.0) return(202);
				Link[index].Kc = value;
				if (Formflag == DW) Link[index].Kc /= (1000.0 * Ucf[ELEV]);
				resistance(index);
			}
			break;

		case EN_MINORLOSS:
			if (Link[index].Type != PUMP)
			{
				if (value <= 0.0) return(202);
				Link[index].Km = 0.02517 * value / SQR(Link[index].Diam) / SQR(Link[index].Diam);
			}
			break;

		case EN_INITSTATUS:
		case EN_STATUS:
			/* Cannot set status for a check valve */
			if (Link[index].Type == CV) return(207);
			s = (char)ROUND(value);
			if (s < 0 || s > 1) return(251);
			if (code == EN_INITSTATUS)
				setlinkstatus(index, s, &Link[index].Stat, &Link[index].Kc);
			else
				setlinkstatus(index, s, &S[index], &K[index]);
			break;

		case EN_INITSETTING:
		case EN_SETTING:
			if (value < 0.0) return(202);
			if (Link[index].Type == PIPE || Link[index].Type == CV)
				return(ENsetlinkvalue(index, EN_ROUGHNESS, v));
			else
			{
				switch (Link[index].Type)
				{
				case PUMP: break;
				case PRV:
				case PSV:
				case PBV: value /= Ucf[PRESSURE]; break;
				case FCV: value /= Ucf[FLOW]; break;
				case TCV: break;

					/***  Updated 9/7/00  ***/
				case GPV: return(202);  /* Cannot modify setting for GPV */

				default:  return(251);
				}
				if (code == EN_INITSETTING)
					setlinksetting(index, value, &Link[index].Stat, &Link[index].Kc);
				else
					setlinksetting(index, value, &S[index], &K[index]);
			}
			break;

		case EN_KBULK:
			if (Link[index].Type <= PIPE)
			{
				Link[index].Kb = value / SECperDAY;
				Reactflag = 1;                                                     //(2.00.12 - LR)
			}
			break;

		case EN_KWALL:
			if (Link[index].Type <= PIPE)
			{
				Link[index].Kw = value / SECperDAY;
				Reactflag = 1;                                                     //(2.00.12 - LR)
			}
			break;

		default: return(251);
		}
		return(0);
	}


	int  EPANET::ENaddpattern(char *id)
		/*----------------------------------------------------------------
		**   Input:   id = ID name of the new pattern
		**   Output:  none
		**   Returns: error code
		**   Purpose: adds a new time pattern appended to the end of the
		**            existing patterns.
		**----------------------------------------------------------------
		*/
	{
		int i, j, n, err = 0;
		Spattern *tmpPat;

		/* Check if a pattern with same id already exists */

		if (!Openflag) return(102);
		if (ENgetpatternindex(id, &i) == 0) return(215);

		/* Check that id name is not too long */

		if (strlen(id) > MAXID) return(250);

		/* Allocate memory for a new array of patterns */

		n = Npats + 1;
		tmpPat = (Spattern *)calloc(n + 1, sizeof(Spattern));
		if (tmpPat == NULL) return(101);

		/* Copy contents of old pattern array to new one */

		for (i = 0; i <= Npats; i++)
		{
			strcpy(tmpPat[i].ID, Pattern[i].ID);
			tmpPat[i].Length = Pattern[i].Length;
			tmpPat[i].F = (double *)calloc(Pattern[i].Length, sizeof(double));
			if (tmpPat[i].F == NULL) err = 1;
			else for (j = 0; j < Pattern[i].Length; j++)
				tmpPat[i].F[j] = Pattern[i].F[j];
		}

		/* Add the new pattern to the new array of patterns */

		strcpy(tmpPat[n].ID, id);
		tmpPat[n].Length = 1;
		tmpPat[n].F = (double *)calloc(tmpPat[n].Length, sizeof(double));
		if (tmpPat[n].F == NULL) err = 1;
		else tmpPat[n].F[0] = 1.0;

		/* Abort if memory allocation error */

		if (err)
		{
			for (i = 0; i <= n; i++) if (tmpPat[i].F) free(tmpPat[i].F);
			free(tmpPat);
			return(101);
		}

		// Replace old pattern array with new one

		for (i = 0; i <= Npats; i++) free(Pattern[i].F);
		free(Pattern);
		Pattern = tmpPat;
		Npats = n;
		MaxPats = n;
		return 0;
	}


	int  EPANET::ENsetpattern(int index, float *f, int n)
		/*----------------------------------------------------------------
		**   Input:   index = time pattern index
		**            *f    = array of pattern multipliers
		**            n     = number of time periods in pattern
		**   Output:  none
		**   Returns: error code
		**   Purpose: sets multipliers for a specific time pattern
		**----------------------------------------------------------------
		*/
	{
		int j;

		/* Check for valid arguments */
		if (!Openflag) return(102);
		if (index <= 0 || index > Npats) return(205);
		if (n <= 0) return(202);

		/* Re-set number of time periods & reallocate memory for multipliers */
		Pattern[index].Length = n;
		Pattern[index].F = (double *)realloc(Pattern[index].F, n * sizeof(double));
		if (Pattern[index].F == NULL) return(101);

		/* Load multipliers into pattern */
		for (j = 0; j < n; j++) Pattern[index].F[j] = f[j];
		return(0);
	}


	int  EPANET::ENsetpatternvalue(int index, int period, float value)
		/*----------------------------------------------------------------
		**  Input:   index  = time pattern index
		**           period = time pattern period
		**           value  = pattern multiplier
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets multiplier for a specific time period and pattern
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		if (index <= 0 || index > Npats) return(205);
		if (period <= 0 || period > Pattern[index].Length) return(251);
		Pattern[index].F[period - 1] = value;
		return(0);
	}


	int  EPANET::ENsettimeparam(int code, long value)
		/*----------------------------------------------------------------
		**  Input:   code  = time parameter code (see TOOLKIT.H)
		**           value = time parameter value
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets value for time parameter
		**----------------------------------------------------------------
		*/
	{
		if (!Openflag) return(102);
		if (OpenHflag || OpenQflag) return(109);
		if (value < 0) return(202);
		switch (code)
		{
		case EN_DURATION:      Dur = value;
			if (Rstart > Dur) Rstart = 0;
			break;
		case EN_HYDSTEP:       if (value == 0) return(202);
			Hstep = value;
			Hstep = MIN(Pstep, Hstep);
			Hstep = MIN(Rstep, Hstep);
			Qstep = MIN(Qstep, Hstep);
			break;
		case EN_QUALSTEP:      if (value == 0) return(202);
			Qstep = value;
			Qstep = MIN(Qstep, Hstep);
			break;
		case EN_PATTERNSTEP:   if (value == 0) return(202);
			Pstep = value;
			if (Hstep > Pstep) Hstep = Pstep;
			break;
		case EN_PATTERNSTART:  Pstart = value;
			break;
		case EN_REPORTSTEP:    if (value == 0) return(202);
			Rstep = value;
			if (Hstep > Rstep) Hstep = Rstep;
			break;
		case EN_REPORTSTART:   if (Rstart > Dur) return(202);
			Rstart = value;
			break;
		case EN_RULESTEP:      if (value == 0) return(202);
			Rulestep = value;
			Rulestep = MIN(Rulestep, Hstep);
			break;
		case EN_STATISTIC:     if (value > RANGE) return(202);
			Tstatflag = (char)value;
			break;
		default:               return(251);
		}
		return(0);
	}


	int  EPANET::ENsetoption(int code, float v)
		/*----------------------------------------------------------------
		**  Input:   code  = option code (see TOOLKIT.H)
		**           v = option value
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets value for an analysis option
		**----------------------------------------------------------------
		*/
	{
		int   i, j;
		double Ke, n, ucf, value = v;
		if (!Openflag) return(102);
		switch (code)
		{
		case EN_TRIALS:     if (value < 1.0) return(202);
			MaxIter = (int)value;
			break;
		case EN_ACCURACY:   if (value < 1.e-5 || value > 1.e-1) return(202);
			Hacc = value;
			break;
		case EN_TOLERANCE:  if (value < 0.0) return(202);
			Ctol = value / Ucf[QUALITY];
			break;
		case EN_EMITEXPON:  if (value <= 0.0) return(202);
			n = 1.0 / value;
			ucf = pow(Ucf[FLOW], n) / Ucf[PRESSURE];
			for (i = 1; i <= Njuncs; i++)
			{
				j = ENgetnodevalue(i, EN_EMITTER, &v);
				Ke = v;
				if (j == 0 && Ke > 0.0) Node[i].Ke = ucf / pow(Ke, n);
			}
			Qexp = n;
			break;
		case EN_DEMANDMULT: if (value <= 0.0) return(202);
			Dmult = value;
			break;
		default:            return(251);
		}
		return(0);
	}


	int  EPANET::ENsetstatusreport(int code)
		/*----------------------------------------------------------------
		**  Input:   code = status reporting code (0, 1, or 2)
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets level of hydraulic status reporting
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;
		if (code >= 0 && code <= 2) Statflag = (char)code;
		else errcode = 202;
		return(errcode);
	}


	int  EPANET::ENsetqualtype(int qualcode, const char *chemname,
		const char *chemunits, const char *tracenode)
		/*----------------------------------------------------------------
		**  Input:   qualcode  = WQ parameter code (see TOOLKIT.H)
		**           chemname  = name of WQ constituent
		**           chemunits = concentration units of WQ constituent
		**           tracenode = ID of node being traced
		**  Output:  none
		**  Returns: error code
		**  Purpose: sets type of quality analysis called for
		**
		**  NOTE: chemname and chemunits only apply when WQ analysis
		**        is for chemical. tracenode only applies when WQ
		**        analysis is source tracing.
		**----------------------------------------------------------------
		*/
	{
		/*** Updated 3/1/01 ***/
		double ccf = 1.0;

		if (!Openflag) return(102);
		if (qualcode < EN_NONE || qualcode > EN_TRACE) return(251);
		Qualflag = (char)qualcode;
		if (Qualflag == CHEM)                   /* Chemical constituent */
		{
			strncpy(ChemName, chemname, MAXID);
			strncpy(ChemUnits, chemunits, MAXID);

			/*** Updated 3/1/01 ***/
			strncpy(Field[QUALITY].Units, ChemUnits, MAXID);
			strncpy(Field[REACTRATE].Units, ChemUnits, MAXID);
			strcat(Field[REACTRATE].Units, t_PERDAY);
			ccf = 1.0 / LperFT3;

		}
		if (Qualflag == TRACE)                  /* Source tracing option */
		{
			char tmp_tracenode[MAXID + 1];
			strncpy(tmp_tracenode, tracenode, MAXID);
			TraceNode = findnode(tmp_tracenode);
			if (TraceNode == 0) return(203);
			strncpy(ChemName, u_PERCENT, MAXID);
			strncpy(ChemUnits, tracenode, MAXID);

			/*** Updated 3/1/01 ***/
			strcpy(Field[QUALITY].Units, u_PERCENT);
		}
		if (Qualflag == AGE)                    /* Water age analysis */
		{
			strncpy(ChemName, w_AGE, MAXID);
			strncpy(ChemUnits, u_HOURS, MAXID);

			/*** Updated 3/1/01 ***/
			strcpy(Field[QUALITY].Units, u_HOURS);
		}

		/*** Updated 3/1/01 ***/
		Ucf[QUALITY] = ccf;
		Ucf[LINKQUAL] = ccf;
		Ucf[REACTRATE] = ccf;

		return(0);
	}


	/*
	----------------------------------------------------------------
	   Functions for opening files
	----------------------------------------------------------------
	*/


	int   EPANET::openfiles(const char *f1, const char *f2, const char *f3)
		/*----------------------------------------------------------------
		**  Input:   f1 = pointer to name of input file
		**           f2 = pointer to name of report file
		**           f3 = pointer to name of binary output file
		**  Output:  none
		**  Returns: error code
		**  Purpose: opens input & report files
		**----------------------------------------------------------------
		*/
	{
		/* Initialize file pointers to NULL */
		InFile = NULL;
		RptFile = NULL;
		OutFile = NULL;
		HydFile = NULL;

		/* Save file names */
		strncpy(InpFname, f1, MAXFNAME);
		strncpy(Rpt1Fname, f2, MAXFNAME);
		strncpy(OutFname, f3, MAXFNAME);
		if (strlen(f3) > 0) Outflag = SAVE;                                         //(2.00.12 - LR)
		else Outflag = SCRATCH;                                                     //(2.00.12 - LR)

	 ///* Check that file names are not identical */
		//if (strcomp(f1, f2) || strcomp(f1, f3) || strcomp(f2, f3))
		//{
		//	writecon(FMT04);
		//	return(301);
		//}

		/* Attempt to open input and report files */
		if ((InFile = fopen(f1, "rt")) == NULL)
		{
			writecon(FMT05);
			writecon(f1);
			return(302);
		}
		if (strlen(f2) == 0) {
			RptFile = stdout;
		}
		else if ((RptFile = fopen(f2, "wt")) == NULL)
		{
			writecon(FMT06);
			return(303);
		}

		return(0);
	}                                       /* End of openfiles */


	int  EPANET::openhydfile()
		/*----------------------------------------------------------------
		** Input:   none
		** Output:  none
		** Returns: error code
		** Purpose: opens file that saves hydraulics solution
		**----------------------------------------------------------------
		*/
	{
		INT4 nsize[6];                       /* Temporary array */
		INT4 magic;
		INT4 version;
		int errcode = 0;

		/* If HydFile currently open, then close it if its not a scratch file */
		if (HydFile != NULL)
		{
			if (Hydflag == SCRATCH) return(0);
			fclose(HydFile);
		}

		/* Use Hydflag to determine the type of hydraulics file to use. */
		/* Write error message if the file cannot be opened.            */
		HydFile = NULL;
		switch (Hydflag)
		{
			/*case SCRATCH:   getTmpName(HydFname);                                     //(2.00.12 - LR)
							HydFile = fopen(HydFname, "w+b");                         //(2.00.12 - LR)
							break;*/

		case SCRATCH:  HydFile = tmpfile();
			break;
		case SAVE:     HydFile = fopen(HydFname, "w+b");
			break;
		case USE:      HydFile = fopen(HydFname, "rb");
			break;
		}
		if (HydFile == NULL) return(305);

		/* If a previous hydraulics solution is not being used, then */
		/* save the current network size parameters to the file.     */
		if (Hydflag != USE)
		{
			magic = MAGICNUMBER;
			version = VERSION;
			nsize[0] = Nnodes;
			nsize[1] = Nlinks;
			nsize[2] = Ntanks;
			nsize[3] = Npumps;
			nsize[4] = Nvalves;
			nsize[5] = Dur;
			fwrite(&magic, sizeof(INT4), 1, HydFile);
			fwrite(&version, sizeof(INT4), 1, HydFile);
			fwrite(nsize, sizeof(INT4), 6, HydFile);
		}

		/* If a previous hydraulics solution is being used, then */
		/* make sure its network size parameters match those of  */
		/* the current network.                                  */
		if (Hydflag == USE)
		{
			fread(&magic, sizeof(INT4), 1, HydFile);
			if (magic != MAGICNUMBER) return(306);
			fread(&version, sizeof(INT4), 1, HydFile);
			if (version != VERSION) return(306);
			if (fread(nsize, sizeof(INT4), 6, HydFile) < 6) return(306);
			if (nsize[0] != Nnodes || nsize[1] != Nlinks ||
				nsize[2] != Ntanks || nsize[3] != Npumps ||
				nsize[4] != Nvalves || nsize[5] != Dur) return(306);
			SaveHflag = TRUE;
		}

		/* Save current position in hydraulics file  */
		/* where storage of hydraulic results begins */
		HydOffset = ftell(HydFile);
		return(errcode);
	}


	int  EPANET::openoutfile()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: opens binary output file.
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;

		/* Close output file if already opened */
		if (OutFile != NULL) fclose(OutFile);
		OutFile = NULL;
		if (TmpOutFile != NULL) fclose(TmpOutFile);
		TmpOutFile = NULL;

		if (Outflag == SCRATCH) remove(OutFname);                                   //(2.00.12 - LR)
		remove(TmpFname);                                                           //(2.00.12 - LR)

	 /* If output file name was supplied, then attempt to */
	 /* open it. Otherwise open a temporary output file.  */
		//if (strlen(OutFname) != 0)                                                //(2.00.12 - LR)
		if (Outflag == SAVE)                                                        //(2.00.12 - LR)
		{
			if ((OutFile = fopen(OutFname, "w+b")) == NULL)
			{
				writecon(FMT07);
				errcode = 304;
			}
		}
		//else if ( (OutFile = tmpfile()) == NULL)                                  //(2.00.12 - LR)
		else                                                                        //(2.00.12 - LR)
		{
			getTmpName(OutFname);                                                    //(2.00.12 - LR)
			if ((OutFile = fopen(OutFname, "w+b")) == NULL)                          //(2.00.12 - LR)
			{
				writecon(FMT08);
				errcode = 304;
			}
		}

		/* Save basic network data & energy usage results */
		ERRCODE(savenetdata());
		OutOffset1 = ftell(OutFile);
		ERRCODE(saveenergy());
		OutOffset2 = ftell(OutFile);

		/* Open temporary file if computing time series statistic */
		if (!errcode)
		{
			if (Tstatflag != SERIES)
			{
				//if ( (TmpOutFile = tmpfile()) == NULL) errcode = 304;               //(2.00.12 - LR)
				getTmpName(TmpFname);                                                 //(2.00.12 - LR)
				TmpOutFile = fopen(TmpFname, "w+b");                                  //(2.00.12 - LR)
				if (TmpOutFile == NULL) errcode = 304;                                //(2.00.12 - LR)
			}
			else TmpOutFile = OutFile;
		}
		return(errcode);
	}


	/*
	----------------------------------------------------------------
	   Global memory management functions
	----------------------------------------------------------------
	*/


	void EPANET::initpointers()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: initializes global pointers to NULL
		**----------------------------------------------------------------
		*/
	{
		D = NULL;
		C = NULL;
		H = NULL;
		Q = NULL;
		R = NULL;
		S = NULL;
		K = NULL;
		OldStat = NULL;

		Node = NULL;
		Link = NULL;
		Tank = NULL;
		Pump = NULL;
		Valve = NULL;
		Pattern = NULL;
		Curve = NULL;
		Control = NULL;

		X = NULL;
		Patlist = NULL;
		Curvelist = NULL;
		Adjlist = NULL;
		Aii = NULL;
		Aij = NULL;
		F = NULL;
		P = NULL;
		Y = NULL;
		Order = NULL;
		Row = NULL;
		Ndx = NULL;
		XLNZ = NULL;
		NZSUB = NULL;
		LNZ = NULL;
		Nht = NULL;
		Lht = NULL;
		initrules();
	}


	int  EPANET::allocdata()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Returns: error code
		**  Purpose: allocates memory for network data structures
		**----------------------------------------------------------------
		*/
	{
		int n;
		int errcode = 0;

		/* Allocate node & link ID hash tables */
		Nht = HTcreate();
		Lht = HTcreate();
		ERRCODE(MEMCHECK(Nht));
		ERRCODE(MEMCHECK(Lht));

		/* Allocate memory for network nodes */
		/*************************************************************
		 NOTE: Because network components of a given type are indexed
			   starting from 1, their arrays must be sized 1
			   element larger than the number of components.
		*************************************************************/
		if (!errcode)
		{
			n = MaxNodes + 1;
			Node = (Snode *)calloc(n, sizeof(Snode));
			D = (double *)calloc(n, sizeof(double));
			C = (double *)calloc(n, sizeof(double));
			H = (double *)calloc(n, sizeof(double));
			ERRCODE(MEMCHECK(Node));
			ERRCODE(MEMCHECK(D));
			ERRCODE(MEMCHECK(C));
			ERRCODE(MEMCHECK(H));
		}

		/* Allocate memory for network links */
		if (!errcode)
		{
			n = MaxLinks + 1;
			Link = (Slink *)calloc(n, sizeof(Slink));
			Q = (double *)calloc(n, sizeof(double));
			K = (double *)calloc(n, sizeof(double));
			S = (char *)calloc(n, sizeof(char));
			ERRCODE(MEMCHECK(Link));
			ERRCODE(MEMCHECK(Q));
			ERRCODE(MEMCHECK(K));
			ERRCODE(MEMCHECK(S));
		}

		/* Allocate memory for tanks, sources, pumps, valves,   */
		/* controls, demands, time patterns, & operating curves */
		if (!errcode)
		{
			Tank = (Stank *)calloc(MaxTanks + 1, sizeof(Stank));
			Pump = (Spump *)calloc(MaxPumps + 1, sizeof(Spump));
			Valve = (Svalve *)calloc(MaxValves + 1, sizeof(Svalve));
			Control = (Scontrol *)calloc(MaxControls + 1, sizeof(Scontrol));
			Pattern = (Spattern *)calloc(MaxPats + 1, sizeof(Spattern));
			Curve = (Scurve *)calloc(MaxCurves + 1, sizeof(Scurve));
			ERRCODE(MEMCHECK(Tank));
			ERRCODE(MEMCHECK(Pump));
			ERRCODE(MEMCHECK(Valve));
			ERRCODE(MEMCHECK(Control));
			ERRCODE(MEMCHECK(Pattern));
			ERRCODE(MEMCHECK(Curve));
		}

		/* Initialize pointers used in patterns, curves, and demand category lists */
		if (!errcode)
		{
			for (n = 0; n <= MaxPats; n++)
			{
				Pattern[n].Length = 0;
				Pattern[n].F = NULL;
			}
			for (n = 0; n <= MaxCurves; n++)
			{
				Curve[n].Npts = 0;
				Curve[n].Type = -1;
				Curve[n].X = NULL;
				Curve[n].Y = NULL;
			}
			for (n = 0; n <= MaxNodes; n++) Node[n].D = NULL;
		}

		/* Allocate memory for rule base (see RULES.C) */
		if (!errcode) errcode = allocrules();
		return(errcode);
	}                                       /* End of allocdata */


	void  EPANET::freeTmplist(STmplist *t)
		/*----------------------------------------------------------------
		**  Input:   t = pointer to start of a temporary list
		**  Output:  none
		**  Purpose: frees memory used for temporary storage
		**           of pattern & curve data
		**----------------------------------------------------------------
		*/
	{
		STmplist *tnext;
		while (t != NULL)
		{
			tnext = t->next;
			freeFloatlist(t->x);
			freeFloatlist(t->y);
			free(t);
			t = tnext;
		}
	}


	void  EPANET::freeFloatlist(SFloatlist *f)
		/*----------------------------------------------------------------
		**  Input:   f = pointer to start of list of floats
		**  Output:  none
		**  Purpose: frees memory used for storing list of floats
		**----------------------------------------------------------------
		*/
	{
		SFloatlist *fnext;
		while (f != NULL)
		{
			fnext = f->next;
			free(f);
			f = fnext;
		}
	}


	void  EPANET::freedata()
		/*----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: frees memory allocated for network data structures.
		**----------------------------------------------------------------
		*/
	{
		int j;
		Pdemand demand, nextdemand;
		Psource source;

		/* Free memory for computed results */
		free(D);
		free(C);
		free(H);
		free(Q);
		free(K);
		free(S);

		/* Free memory for node data */
		if (Node != NULL)
		{
			for (j = 0; j <= MaxNodes; j++)
			{
				/* Free memory used for demand category list */
				demand = Node[j].D;
				while (demand != NULL)
				{
					nextdemand = demand->next;
					free(demand);
					demand = nextdemand;
				}
				/* Free memory used for WQ source data */
				source = Node[j].S;
				if (source != NULL) free(source);
			}
			free(Node);
		}

		/* Free memory for other network objects */
		free(Link);
		free(Tank);
		free(Pump);
		free(Valve);
		free(Control);

		/* Free memory for time patterns */
		if (Pattern != NULL)
		{
			for (j = 0; j <= MaxPats; j++) free(Pattern[j].F);
			free(Pattern);
		}

		/* Free memory for curves */
		if (Curve != NULL)
		{
			for (j = 0; j <= MaxCurves; j++)
			{
				free(Curve[j].X);
				free(Curve[j].Y);
			}
			free(Curve);
		}

		/* Free memory for rule base (see RULES.C) */
		freerules();

		/* Free hash table memory */
		if (Nht != NULL) HTfree(Nht);
		if (Lht != NULL) HTfree(Lht);
	}


	/*
	----------------------------------------------------------------
	   General purpose functions
	----------------------------------------------------------------
	*/

	/*** New function for 2.00.12 ***/                                             //(2.00.12 - LR)
	char *EPANET::getTmpName(char *fname)
		//
		//  Input:   fname = file name string
		//  Output:  returns pointer to file name
		//  Purpose: creates a temporary file name with path prepended to it.
		//
	{
		char name[MAXFNAME + 1];
		size_t  n;

		// --- for Windows systems:
#ifdef WINDOWS
  // --- use system function tmpnam() to create a temporary file name
		tmpnam(name);

		// --- if user supplied the name of a temporary directory,
		//     then make it be the prefix of the full file name
		n = strlen(TmpDir);
		if (n > 0)
		{
			strcpy(fname, TmpDir);
			if (fname[n - 1] != '\\') strcat(fname, "\\");
		}

		// --- otherwise, use the relative path notation as the file name
		//     prefix so that the file will be placed in the current directory
		else
		{
			strcpy(fname, ".\\");
		}

		// --- now add the prefix to the file name
		strcat(fname, name);

		// --- for non-Windows systems:
#else
  // --- use system function mkstemp() to create a temporary file name
		strcpy(fname, "enXXXXXX");
		mkstemp(fname);
#endif
		return fname;
	}


	int  EPANET::strcomp(const char *s1,const  char *s2)
		/*---------------------------------------------------------------
		**  Input:   s1 = character string
		**           s2 = character string
		**  Output:  none
		**  Returns: 1 if s1 is same as s2, 0 otherwise
		**  Purpose: case insensitive comparison of strings s1 & s2
		**---------------------------------------------------------------
		*/
	{
		int i;
		for (i = 0; UCHAR(s1[i]) == UCHAR(s2[i]); i++)
			if (!s1[i + 1] && !s2[i + 1]) return(1);
		return(0);
	}                                       /*  End of strcomp  */


	double  EPANET::interp(int n, double *x, double *y, double xx)
		/*----------------------------------------------------------------
		**  Input:   n  = number of data pairs defining a curve
		**           x  = x-data values of curve
		**           y  = y-data values of curve
		**           xx = specified x-value
		**  Output:  none
		**  Returns: y-value on curve at x = xx
		**  Purpose: uses linear interpolation to find y-value on a
		**           data curve corresponding to specified x-value.
		**  NOTE:    does not extrapolate beyond endpoints of curve.
		**----------------------------------------------------------------
		*/
	{
		int    k, m;
		double  dx, dy;

		m = n - 1;                          /* Highest data index      */
		if (xx <= x[0]) return(y[0]);       /* xx off low end of curve */
		for (k = 1; k <= m; k++)                /* Bracket xx on curve     */
		{
			if (x[k] >= xx)                 /* Interp. over interval   */
			{
				dx = x[k] - x[k - 1];
				dy = y[k] - y[k - 1];
				if (ABS(dx) < TINY) return(y[k]);
				else return(y[k] - (x[k] - xx) * dy / dx);
			}
		}
		return(y[m]);                       /* xx off high end of curve */
	}                       /* End of interp */


	int   EPANET::EPANET::findnode(char *id)
		/*----------------------------------------------------------------
		**  Input:   id = node ID
		**  Output:  none
		**  Returns: index of node with given ID, or 0 if ID not found
		**  Purpose: uses hash table to find index of node with given ID
		**----------------------------------------------------------------
		*/
	{
		return(HTfind(Nht, id));
	}


	int  EPANET::findlink(char *id)
		/*----------------------------------------------------------------
		**  Input:   id = link ID
		**  Output:  none
		**  Returns: index of link with given ID, or 0 if ID not found
		**  Purpose: uses hash table to find index of link with given ID
		**----------------------------------------------------------------
		*/
	{
		return(HTfind(Lht, id));
	}


	char *EPANET::geterrmsg(int errcode)
		/*----------------------------------------------------------------
		**  Input:   errcode = error code
		**  Output:  none
		**  Returns: pointer to string with error message
		**  Purpose: retrieves text of error message
		**----------------------------------------------------------------
		*/
	{
		switch (errcode)
		{                                   /* Warnings */
	 /*
		   case 1:     strcpy(Msg,WARN1);   break;
		   case 2:     strcpy(Msg,WARN2);   break;
		   case 3:     strcpy(Msg,WARN3);   break;
		   case 4:     strcpy(Msg,WARN4);   break;
		   case 5:     strcpy(Msg,WARN5);   break;
		   case 6:     strcpy(Msg,WARN6);   break;
	 */
	 /* System Errors */
		case 101:   strcpy(Msg, ERR101);  break;
		case 102:   strcpy(Msg, ERR102);  break;
		case 103:   strcpy(Msg, ERR103);  break;
		case 104:   strcpy(Msg, ERR104);  break;
		case 105:   strcpy(Msg, ERR105);  break;
		case 106:   strcpy(Msg, ERR106);  break;
		case 107:   strcpy(Msg, ERR107);  break;
		case 108:   strcpy(Msg, ERR108);  break;
		case 109:   strcpy(Msg, ERR109);  break;
		case 110:   strcpy(Msg, ERR110);  break;
		case 120:   strcpy(Msg, ERR120);  break;

			/* Input Errors */
		case 200:  strcpy(Msg, ERR200);   break;
		case 223:  strcpy(Msg, ERR223);   break;
		case 224:  strcpy(Msg, ERR224);   break;

			/* Toolkit function errors */
		case 202:  sprintf(Msg, ERR202, t_FUNCCALL, ""); break;
		case 203:  sprintf(Msg, ERR203, t_FUNCCALL, ""); break;
		case 204:  sprintf(Msg, ERR204, t_FUNCCALL, ""); break;
		case 205:  sprintf(Msg, ERR205, t_FUNCCALL, ""); break;
		case 207:  sprintf(Msg, ERR207, t_FUNCCALL, ""); break;
		case 240:  sprintf(Msg, ERR240, t_FUNCCALL, ""); break;
		case 241:  sprintf(Msg, ERR241, t_FUNCCALL, ""); break;
		case 250:  sprintf(Msg, ERR250);  break;
		case 251:  sprintf(Msg, ERR251);  break;

			/* File Errors */
		case 301:  strcpy(Msg, ERR301);   break;
		case 302:  strcpy(Msg, ERR302);   break;
		case 303:  strcpy(Msg, ERR303);   break;
		case 304:  strcpy(Msg, ERR304);   break;
		case 305:  strcpy(Msg, ERR305);   break;
		case 306:  strcpy(Msg, ERR306);   break;
		case 307:  strcpy(Msg, ERR307);   break;
		case 308:  strcpy(Msg, ERR308);   break;
		case 309:  strcpy(Msg, ERR309);   break;
		default:   strcpy(Msg, "");
		}
		return(Msg);
	}


	void  EPANET::errmsg(int errcode)
		/*----------------------------------------------------------------
		**  Input:   errcode = error code
		**  Output:  none
		**  Purpose: writes error message to report file
		**----------------------------------------------------------------
		*/
	{
		if (errcode == 309)    /* Report file write error -  */
		{                      /* Do not write msg to file.  */
			writecon("\n  ");
			writecon(geterrmsg(errcode));
		}
		else if (RptFile != NULL && Messageflag)
		{
			writeline(geterrmsg(errcode));
		}
	}


	void  EPANET::writecon(const char *s)
		/*----------------------------------------------------------------
		**  Input:   text string
		**  Output:  none
		**  Purpose: writes string of characters to console
		**----------------------------------------------------------------
		*/
	{
#ifdef CLE                                                                     //(2.00.11 - LR)
		fprintf(stdout, s);
		fflush(stdout);
#endif
	}


	void EPANET::writewin(const char *s)
		/*----------------------------------------------------------------
		**  Input:   text string
		**  Output:  none
		**  Purpose: passes character string to viewprog() in
		**           application which calls the EPANET DLL
		**----------------------------------------------------------------
		*/
	{
		
#ifdef DLL
		char progmsg[MAXMSG + 1];
		if (viewprog != NULL)
		{
			strncpy(progmsg, s, MAXMSG);
			viewprog(progmsg);
		}
#endif
	}

	char *gStrtok_r(char *s, const char *delim, char **save_ptr) {

		if (s == 0) s = *save_ptr;

		/* Scan leading delimiters.  */
		s += strspn(s, delim);
		if (*s == '\0') return 0;

		/* Find the end of the token.  */
		char *token = s;
		s = strpbrk(token, delim);
		if (s == 0)
			/* This token finishes the string.  */
			*save_ptr = strchr(token, '\0');
		else {
			/* kTerminate the token and make *SAVE_PTR point past it.  */
			*s = '\0';
			*save_ptr = s + 1;
		}

		return token;
	}

	/*input1*/
	int  EPANET::getdata()
		/*
		**----------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: reads in network data from disk file
		**----------------------------------------------------------------
		*/
	{
		int errcode = 0;
		setdefaults();                /* Assign default data values     */
		initreport();                 /* Initialize reporting options   */
		rewind(InFile);               /* Rewind input file              */
		ERRCODE(readdata());          /* Read in network data           */
		if (!errcode) adjustdata();   /* Adjust data for default values */
		if (!errcode) initunits();    /* Initialize units on input data */
		ERRCODE(inittanks());         /* Initialize tank volumes        */
		if (!errcode) convertunits(); /* Convert units on input data    */
		return(errcode);
	}                       /*  End of getdata  */


	void  EPANET::setdefaults()
		/*
		**----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: assigns default values to global variables
		**----------------------------------------------------------------
		*/
	{
		strncpy(Title[0], "", MAXMSG);
		strncpy(Title[1], "", MAXMSG);
		strncpy(Title[2], "", MAXMSG);
		strncpy(TmpDir, "", MAXFNAME);                                                //(2.00.12 - LR)
		strncpy(TmpFname, "", MAXFNAME);                                              //(2.00.12 - LR)
		strncpy(HydFname, "", MAXFNAME);
		strncpy(MapFname, "", MAXFNAME);
		strncpy(ChemName, t_CHEMICAL, MAXID);
		strncpy(ChemUnits, u_MGperL, MAXID);
		strncpy(DefPatID, DEFPATID, MAXID);
		Hydflag = SCRATCH;         /* No external hydraulics file    */
		Qualflag = NONE;            /* No quality simulation          */
		Formflag = HW;              /* Use Hazen-Williams formula     */
		Unitsflag = US;              /* US unit system                 */
		Flowflag = GPM;             /* Flow units are gpm             */
		Pressflag = PSI;             /* Pressure units are psi         */
		Tstatflag = SERIES;          /* Generate time series output    */
		Warnflag = FALSE;           /* Warning flag is off            */
		Htol = HTOL;            /* Default head tolerance         */
		Qtol = QTOL;            /* Default flow tolerance         */
		Hacc = HACC;            /* Default hydraulic accuracy     */
		Ctol = MISSING;         /* No pre-set quality tolerance   */
		MaxIter = MAXITER;         /* Default max. hydraulic trials  */
		ExtraIter = -1;              /* Stop if network unbalanced     */
		Dur = 0;               /* 0 sec duration (steady state)  */
		Tstart = 0;               /* Starting time of day           */
		Pstart = 0;               /* Starting pattern period        */
		Hstep = 3600;            /* 1 hr hydraulic time Step       */
		Qstep = 0;               /* No pre-set quality time Step   */
		Pstep = 3600;            /* 1 hr time pattern period       */
		Rstep = 3600;            /* 1 hr reporting period          */
		Rulestep = 0;               /* No pre-set rule time Step      */
		Rstart = 0;               /* Start reporting at time 0      */
		TraceNode = 0;               /* No source tracing              */
		BulkOrder = 1.0;             /* 1st-order bulk reaction rate   */
		WallOrder = 1.0;             /* 1st-order wall reaction rate   */
		TankOrder = 1.0;             /* 1st-order tank reaction rate   */
		Kbulk = 0.0;             /* No global bulk reaction        */
		Kwall = 0.0;             /* No global wall reaction        */
		Climit = 0.0;             /* No limiting potential quality  */
		Diffus = MISSING;         /* Temporary diffusivity          */
		Rfactor = 0.0;             /* No roughness-reaction factor   */
		Viscos = MISSING;         /* Temporary viscosity            */
		SpGrav = SPGRAV;          /* Default specific gravity       */
		DefPat = 0;               /* Default demand pattern index   */
		Epat = 0;               /* No energy price pattern        */
		Ecost = 0.0;             /* Zero unit energy cost          */
		Dcost = 0.0;             /* Zero energy demand charge      */
		Epump = EPUMP;           /* Default pump efficiency        */
		Emax = 0.0;             /* Zero peak energy usage         */
		Qexp = 2.0;             /* Flow exponent for emitters     */
		Dmult = 1.0;             /* Demand multiplier              */
		RQtol = RQTOL;           /* Default hydraulics parameters  */
		CheckFreq = CHECKFREQ;
		MaxCheck = MAXCHECK;
		DampLimit = DAMPLIMIT;                                                      //(2.00.12 - LR)
	}                       /*  End of setdefaults  */


	void  EPANET::initreport()
		/*
		**----------------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: initializes reporting options
		**----------------------------------------------------------------------
		*/
	{
		int i;
		strncpy(Rpt2Fname, "", MAXFNAME);
		PageSize = PAGESIZE;      /* Default page size for report   */
		Summaryflag = TRUE;          /* Write summary report           */
		Messageflag = TRUE;          /* Report error/warning messages  */
		Statflag = FALSE;         /* No hydraulic status reports    */
		Energyflag = FALSE;         /* No energy usage report         */
		Nodeflag = 0;             /* No reporting on nodes          */
		Linkflag = 0;             /* No reporting on links          */
		for (i = 0; i < MAXVAR; i++)     /* For each reporting variable:   */
		{
			strncpy(Field[i].Name, Fldname[i], MAXID);
			Field[i].Enabled = FALSE;        /* Not included in report  */
			Field[i].Precision = 2;          /* 2 decimal precision     */

											 /*** Updated 6/24/02 ***/
			Field[i].RptLim[LOW] = SQR(BIG); /* No reporting limits   */
			Field[i].RptLim[HI] = -SQR(BIG);
		}
		Field[FRICTION].Precision = 3;
		for (i = DEMAND; i <= QUALITY; i++) Field[i].Enabled = TRUE;
		for (i = FLOW; i <= HEADLOSS; i++) Field[i].Enabled = TRUE;
	}


	void  EPANET::adjustdata()
		/*
		**----------------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: adjusts data after input file has been processed
		**----------------------------------------------------------------------
		*/
	{
		int   i;
		double ucf;                   /* Unit conversion factor */
		Pdemand demand;              /* Pointer to demand record */

									 /* Use 1 hr pattern & report time Step if none specified */
		if (Pstep <= 0) Pstep = 3600;
		if (Rstep == 0) Rstep = Pstep;

		/* Hydraulic time Step cannot be greater than pattern or report time Step */
		if (Hstep <= 0)   Hstep = 3600;
		if (Hstep > Pstep) Hstep = Pstep;
		if (Hstep > Rstep) Hstep = Rstep;

		/* Report start time cannot be greater than simulation duration */
		if (Rstart > Dur) Rstart = 0;

		/* No water quality analysis for steady state run */
		if (Dur == 0) Qualflag = NONE;

		/* If no quality timestep, then make it 1/10 of hydraulic timestep */
		if (Qstep == 0) Qstep = Hstep / 10;

		/* If no rule time Step then make it 1/10 of hydraulic time Step; */
		/* Rule time Step cannot be greater than hydraulic time Step */
		if (Rulestep == 0) Rulestep = Hstep / 10;
		Rulestep = MIN(Rulestep, Hstep);

		/* Quality timestep cannot exceed hydraulic timestep */
		Qstep = MIN(Qstep, Hstep);

		/* If no quality tolerance, then use default values */
		if (Ctol == MISSING)
		{
			if (Qualflag == AGE) Ctol = AGETOL;
			else                 Ctol = CHEMTOL;
		}

		/* Determine unit system based on flow units */
		switch (Flowflag)
		{
		case LPS:          /* Liters/sec */
		case LPM:          /* Liters/min */
		case MLD:          /* megaliters/day  */
		case CMH:          /* cubic meters/hr */
		case CMD:          /* cubic meters/day */
			Unitsflag = SI;
			break;
		default:
			Unitsflag = US;
		}

		/* Revise pressure units depending on flow units */
		if (Unitsflag != SI) Pressflag = PSI;
		else if (Pressflag == PSI) Pressflag = METERS;

		/* Store value of viscosity & diffusivity */
		ucf = 1.0;
		if (Unitsflag == SI) ucf = SQR(MperFT);

		if (Viscos == MISSING)     /* No value supplied */
			Viscos = VISCOS;
		else if (Viscos > 1.e-3)   /* Multiplier supplied */
			Viscos = Viscos * VISCOS;
		else                       /* Actual value supplied */
			Viscos = Viscos / ucf;

		if (Diffus == MISSING)
			Diffus = DIFFUS;
		else if (Diffus > 1.e-4)
			Diffus = Diffus * DIFFUS;
		else
			Diffus = Diffus / ucf;

		/*
		Set exponent in head loss equation and adjust flow-resistance tolerance.
		*/
		if (Formflag == HW) Hexp = 1.852;
		else                Hexp = 2.0;

		/*** Updated 9/7/00 ***/
		/*** No adjustment made to flow-resistance tolerance ***/
		/*RQtol = RQtol/Hexp;*/

		/* See if default reaction coeffs. apply */
		for (i = 1; i <= Nlinks; i++)
		{
			if (Link[i].Type > PIPE) continue;
			if (Link[i].Kb == MISSING) Link[i].Kb = Kbulk;  /* Bulk coeff. */
			if (Link[i].Kw == MISSING)                      /* Wall coeff. */
			{
				/* Rfactor is the pipe roughness correlation factor */
				if (Rfactor == 0.0)   Link[i].Kw = Kwall;
				else if ((Link[i].Kc > 0.0) && (Link[i].Diam > 0.0))
				{
					if (Formflag == HW) Link[i].Kw = Rfactor / Link[i].Kc;
					if (Formflag == DW) Link[i].Kw = Rfactor / ABS(log(Link[i].Kc / Link[i].Diam));
					if (Formflag == CM) Link[i].Kw = Rfactor * Link[i].Kc;
				}
				else Link[i].Kw = 0.0;
			}
		}
		for (i = 1; i <= Ntanks; i++)
			if (Tank[i].Kb == MISSING) Tank[i].Kb = Kbulk;

		/* Use default pattern if none assigned to a demand */
		for (i = 1; i <= Nnodes; i++)
		{
			for (demand = Node[i].D; demand != NULL; demand = demand->next)
				if (demand->Pat == 0) demand->Pat = DefPat;
		}

		/* Remove QUALITY as a reporting variable if no WQ analysis */
		if (Qualflag == NONE) Field[QUALITY].Enabled = FALSE;

	}                       /*  End of adjustdata  */


	int  EPANET::inittanks()
		/*
		**---------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: initializes volumes in non-cylindrical tanks
		**---------------------------------------------------------------
		*/
	{
		int   i, j, n = 0;
		double a;
		int   errcode = 0,
			levelerr;

		for (j = 1; j <= Ntanks; j++)
		{

			/* Skip reservoirs */
			if (Tank[j].A == 0.0) continue;

			/* Check for valid lower/upper tank levels */
			levelerr = 0;
			if (Tank[j].H0 > Tank[j].Hmax ||
				Tank[j].Hmin > Tank[j].Hmax ||
				Tank[j].H0 < Tank[j].Hmin
				) levelerr = 1;

			/* Check that tank heights are within volume curve */
			i = Tank[j].Vcurve;
			if (i > 0)
			{
				n = Curve[i].Npts - 1;
				if (Tank[j].Hmin < Curve[i].X[0] ||
					Tank[j].Hmax > Curve[i].X[n]
					) levelerr = 1;
			}

			/* Report error in levels if found */
			if (levelerr)
			{
				sprintf(Msg, ERR225, Node[Tank[j].Node].ID);
				writeline(Msg);
				errcode = 200;
			}

			/* Else if tank has a volume curve, */
			else if (i > 0)
			{
				/* Find min., max., and initial volumes from curve */
				Tank[j].Vmin = interp(Curve[i].Npts, Curve[i].X,
					Curve[i].Y, Tank[j].Hmin);
				Tank[j].Vmax = interp(Curve[i].Npts, Curve[i].X,
					Curve[i].Y, Tank[j].Hmax);
				Tank[j].V0 = interp(Curve[i].Npts, Curve[i].X,
					Curve[i].Y, Tank[j].H0);

				/* Find a "nominal" diameter for tank */
				a = (Curve[i].Y[n] - Curve[i].Y[0]) /
					(Curve[i].X[n] - Curve[i].X[0]);
				Tank[j].A = sqrt(4.0 * a / PI);
			}
		}
		return(errcode);
	}                       /* End of inittanks */


	void  EPANET::initunits()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: determines unit conversion factors
		**--------------------------------------------------------------
		*/
	{
		double  dcf,  /* distance conversion factor      */
			ccf,  /* concentration conversion factor */
			qcf,  /* flow conversion factor          */
			hcf,  /* head conversion factor          */
			pcf,  /* pressure conversion factor      */
			wcf;  /* energy conversion factor        */

		if (Unitsflag == SI)                            /* SI units */
		{
			strcpy(Field[DEMAND].Units, RptFlowUnitsTxt[Flowflag]);
			strcpy(Field[ELEV].Units, u_METERS);
			strcpy(Field[HEAD].Units, u_METERS);
			if (Pressflag == METERS) strcpy(Field[PRESSURE].Units, u_METERS);
			else strcpy(Field[PRESSURE].Units, u_KPA);
			strcpy(Field[LENGTH].Units, u_METERS);
			strcpy(Field[DIAM].Units, u_MMETERS);
			strcpy(Field[FLOW].Units, RptFlowUnitsTxt[Flowflag]);
			strcpy(Field[VELOCITY].Units, u_MperSEC);
			strcpy(Field[HEADLOSS].Units, u_per1000M);
			strcpy(Field[FRICTION].Units, "");
			strcpy(Field[POWER].Units, u_KW);
			dcf = 1000.0 * MperFT;
			qcf = LPSperCFS;
			if (Flowflag == LPM) qcf = LPMperCFS;
			if (Flowflag == MLD) qcf = MLDperCFS;
			if (Flowflag == CMH) qcf = CMHperCFS;
			if (Flowflag == CMD) qcf = CMDperCFS;
			hcf = MperFT;
			if (Pressflag == METERS) pcf = MperFT * SpGrav;
			else pcf = KPAperPSI * PSIperFT * SpGrav;
			wcf = KWperHP;
		}
		else                                         /* US units */
		{
			strcpy(Field[DEMAND].Units, RptFlowUnitsTxt[Flowflag]);
			strcpy(Field[ELEV].Units, u_FEET);
			strcpy(Field[HEAD].Units, u_FEET);
			strcpy(Field[PRESSURE].Units, u_PSI);
			strcpy(Field[LENGTH].Units, u_FEET);
			strcpy(Field[DIAM].Units, u_INCHES);
			strcpy(Field[FLOW].Units, RptFlowUnitsTxt[Flowflag]);
			strcpy(Field[VELOCITY].Units, u_FTperSEC);
			strcpy(Field[HEADLOSS].Units, u_per1000FT);
			strcpy(Field[FRICTION].Units, "");
			strcpy(Field[POWER].Units, u_HP);
			dcf = 12.0;
			qcf = 1.0;
			if (Flowflag == GPM) qcf = GPMperCFS;
			if (Flowflag == MGD) qcf = MGDperCFS;
			if (Flowflag == IMGD)qcf = IMGDperCFS;
			if (Flowflag == AFD) qcf = AFDperCFS;
			hcf = 1.0;
			pcf = PSIperFT * SpGrav;
			wcf = 1.0;
		}
		strcpy(Field[QUALITY].Units, "");
		ccf = 1.0;
		if (Qualflag == CHEM)
		{
			ccf = 1.0 / LperFT3;
			strncpy(Field[QUALITY].Units, ChemUnits, MAXID);
			strncpy(Field[REACTRATE].Units, ChemUnits, MAXID);
			strcat(Field[REACTRATE].Units, t_PERDAY);
		}
		else if (Qualflag == AGE) strcpy(Field[QUALITY].Units, u_HOURS);
		else if (Qualflag == TRACE) strcpy(Field[QUALITY].Units, u_PERCENT);
		Ucf[DEMAND] = qcf;
		Ucf[ELEV] = hcf;
		Ucf[HEAD] = hcf;
		Ucf[PRESSURE] = pcf;
		Ucf[QUALITY] = ccf;
		Ucf[LENGTH] = hcf;
		Ucf[DIAM] = dcf;
		Ucf[FLOW] = qcf;
		Ucf[VELOCITY] = hcf;
		Ucf[HEADLOSS] = hcf;
		Ucf[LINKQUAL] = ccf;
		Ucf[REACTRATE] = ccf;
		Ucf[FRICTION] = 1.0;
		Ucf[POWER] = wcf;
		Ucf[VOLUME] = hcf * hcf * hcf;
		if (Hstep < 1800)                    /* Report time in mins.    */
		{                                    /* if hydraulic time Step  */
			Ucf[TIME] = 1.0 / 60.0;             /* is less than 1/2 hour.  */
			strcpy(Field[TIME].Units, u_MINUTES);
		}
		else
		{
			Ucf[TIME] = 1.0 / 3600.0;
			strcpy(Field[TIME].Units, u_HOURS);
		}

	}                       /*  End of initunits  */


	void  EPANET::convertunits()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: converts units of input data
		**--------------------------------------------------------------
		*/
	{
		int   i, j, k;
		double ucf;        /* Unit conversion factor */
		Pdemand demand;   /* Pointer to demand record */

						  /* Convert nodal elevations & initial WQ */
						  /* (WQ source units are converted in QUALITY.C */
		for (i = 1; i <= Nnodes; i++)
		{
			Node[i].El /= Ucf[ELEV];
			Node[i].C0 /= Ucf[QUALITY];
		}

		/* Convert demands */
		for (i = 1; i <= Njuncs; i++)
		{
			for (demand = Node[i].D; demand != NULL; demand = demand->next)
				demand->Base /= Ucf[DEMAND];
		}

		/* Convert emitter discharge coeffs. to head loss coeff. */
		ucf = pow(Ucf[FLOW], Qexp) / Ucf[PRESSURE];
		for (i = 1; i <= Njuncs; i++)
			if (Node[i].Ke > 0.0) Node[i].Ke = ucf / pow(Node[i].Ke, Qexp);

		/* Initialize tank variables (convert tank levels to elevations) */
		for (j = 1; j <= Ntanks; j++)
		{
			i = Tank[j].Node;
			Tank[j].H0 = Node[i].El + Tank[j].H0 / Ucf[ELEV];
			Tank[j].Hmin = Node[i].El + Tank[j].Hmin / Ucf[ELEV];
			Tank[j].Hmax = Node[i].El + Tank[j].Hmax / Ucf[ELEV];
			Tank[j].A = PI * SQR(Tank[j].A / Ucf[ELEV]) / 4.0;
			Tank[j].V0 /= Ucf[VOLUME];
			Tank[j].Vmin /= Ucf[VOLUME];
			Tank[j].Vmax /= Ucf[VOLUME];
			Tank[j].Kb /= SECperDAY;
			Tank[j].V = Tank[j].V0;
			Tank[j].C = Node[i].C0;
			Tank[j].V1max *= Tank[j].Vmax;
		}

		/* Convert WQ option concentration units */
		Climit /= Ucf[QUALITY];
		Ctol /= Ucf[QUALITY];

		/* Convert global reaction coeffs. */
		Kbulk /= SECperDAY;
		Kwall /= SECperDAY;

		/* Convert units of link parameters */
		for (k = 1; k <= Nlinks; k++)
		{
			if (Link[k].Type <= PIPE)
			{
				/* Convert pipe parameter units:                         */
				/*    - for Darcy-Weisbach formula, convert roughness    */
				/*      from millifeet (or mm) to ft (or m)              */
				/*    - for US units, convert diameter from inches to ft */
				if (Formflag == DW) Link[k].Kc /= (1000.0 * Ucf[ELEV]);
				Link[k].Diam /= Ucf[DIAM];
				Link[k].Len /= Ucf[LENGTH];

				/* Convert minor loss coeff. from V^2/2g basis to Q^2 basis */
				Link[k].Km = 0.02517 * Link[k].Km / SQR(Link[k].Diam) / SQR(Link[k].Diam);

				/* Convert units on reaction coeffs. */
				Link[k].Kb /= SECperDAY;
				Link[k].Kw /= SECperDAY;
			}

			else if (Link[k].Type == PUMP)
			{
				/* Convert units for pump curve parameters */
				i = PUMPINDEX(k);
				if (Pump[i].Ptype == CONST_HP)
				{
					/* For constant hp pump, convert kw to hp */
					if (Unitsflag == SI) Pump[i].R /= Ucf[POWER];
				}
				else
				{
					/* For power curve pumps, convert     */
					/* shutoff head and flow coefficient  */
					if (Pump[i].Ptype == POWER_FUNC)
					{
						Pump[i].H0 /= Ucf[HEAD];
						Pump[i].R *= (pow(Ucf[FLOW], Pump[i].N) / Ucf[HEAD]);
					}
					/* Convert flow range & max. head units */
					Pump[i].Q0 /= Ucf[FLOW];
					Pump[i].Qmax /= Ucf[FLOW];
					Pump[i].Hmax /= Ucf[HEAD];
				}
			}

			else
			{
				/* For flow control valves, convert flow setting    */
				/* while for other valves convert pressure setting  */
				Link[k].Diam /= Ucf[DIAM];
				Link[k].Km = 0.02517 * Link[k].Km / SQR(Link[k].Diam) / SQR(Link[k].Diam);
				if (Link[k].Kc != MISSING) switch (Link[k].Type)
				{
				case FCV: Link[k].Kc /= Ucf[FLOW]; break;
				case PRV:
				case PSV:
				case PBV: Link[k].Kc /= Ucf[PRESSURE]; break;
				}
			}

			/* Compute flow resistances */
			resistance(k);
		}

		/* Convert units on control settings */
		for (i = 1; i <= Ncontrols; i++)
		{
			if ((k = Control[i].Link) == 0) continue;
			if ((j = Control[i].Node) > 0)
			{
				/* j = index of controlling node, and if           */
				/* j > Njuncs, then control is based on tank level */
				/* otherwise control is based on nodal pressure    */
				if (j > Njuncs)
					Control[i].Grade = Node[j].El + Control[i].Grade / Ucf[ELEV];
				else Control[i].Grade = Node[j].El + Control[i].Grade / Ucf[PRESSURE];
			}

			/* Convert units on valve settings */
			if (Control[i].Setting != MISSING) switch (Link[k].Type)
			{
			case PRV:
			case PSV:
			case PBV:
				Control[i].Setting /= Ucf[PRESSURE];
				break;
			case FCV:
				Control[i].Setting /= Ucf[FLOW];
			}
		}
	}                       /*  End of convertunits  */

	/*input2*/

	int  EPANET::netsize()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: determines number of system components
		**--------------------------------------------------------------
		*/
	{
		char  line[MAXLINE + 1];     /* Line from input data file    */
		char *tok;                /* First token of line          */
		int   sect, newsect;        /* Input data sections          */
		int   errcode = 0;         /* Error code                   */

								   /* Initialize network component counts */
		MaxJuncs = 0;
		MaxTanks = 0;
		MaxPipes = 0;
		MaxPumps = 0;
		MaxValves = 0;
		MaxControls = 0;
		MaxRules = 0;
		MaxCurves = 0;
		sect = -1;

		/* Add a default pattern 0 */
		MaxPats = -1;
		addpattern("");

		/* Make pass through data file counting number of each component */
		while (fgets(line, MAXLINE, InFile) != NULL)
		{
			/* Skip blank lines & those beginning with a comment */
			tok = strtok(line, SEPSTR);
			if (tok == NULL) continue;
			if (*tok == ';') continue;

			/* Check if line begins with a new section heading */
			if (*tok == '[')
			{
				newsect = findmatch(tok, SectTxt);
				if (newsect >= 0)
				{
					sect = newsect;
					if (sect == _END) break;
					continue;
				}
				else continue;
			}

			/* Add to count of current component */
			switch (sect)
			{
			case _JUNCTIONS:  MaxJuncs++;    break;
			case _RESERVOIRS:
			case _TANKS:      MaxTanks++;    break;
			case _PIPES:      MaxPipes++;    break;
			case _PUMPS:      MaxPumps++;    break;
			case _VALVES:     MaxValves++;   break;
			case _CONTROLS:   MaxControls++; break;
			case _RULES:      addrule(tok);  break; /* See RULES.C */
			case _PATTERNS:   errcode = addpattern(tok);
				break;
			case _CURVES:     errcode = addcurve(tok);
				break;
			}
			if (errcode) break;
		}

		MaxNodes = MaxJuncs + MaxTanks;
		MaxLinks = MaxPipes + MaxPumps + MaxValves;
		if (MaxPats < 1) MaxPats = 1;
		if (!errcode)
		{
			if (MaxJuncs < 1) errcode = 223;       /* Not enough nodes */
			else if (MaxTanks == 0) errcode = 224; /* No tanks */
		}
		return(errcode);
	}                        /*  End of netsize  */


	int  EPANET::readdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: reads contents of input data file
		**--------------------------------------------------------------
		*/
	{
		char  line[MAXLINE + 1],     /* Line from input data file       */
			wline[MAXLINE + 1];    /* Working copy of input line      */
		int   sect, newsect,        /* Data sections                   */
			errcode = 0,         /* Error code                      */
			inperr, errsum;       /* Error code & total error count  */

								  /* Allocate input buffer */
		X = (double *)calloc(MAXTOKS, sizeof(double));
		ERRCODE(MEMCHECK(X));

		if (!errcode)
		{

			/* Initialize number of network components */
			Ntitle = 0;
			Nnodes = 0;
			Njuncs = 0;
			Ntanks = 0;
			Nlinks = 0;
			Npipes = 0;
			Npumps = 0;
			Nvalves = 0;
			Ncontrols = 0;
			Nrules = 0;
			Ncurves = MaxCurves;
			Npats = MaxPats;
			PrevPat = NULL;
			PrevCurve = NULL;
			sect = -1;
			errsum = 0;

			/* Read each line from input file. */
			while (fgets(line, MAXLINE, InFile) != NULL)
			{

				/* Make copy of line and scan for tokens */
				strcpy(wline, line);
				Ntokens = gettokens(wline);

				/* Skip blank lines and comments */
				if (Ntokens == 0) continue;
				if (*Tok[0] == ';') continue;

				/* Check if max. length exceeded */
				if (strlen(line) >= MAXLINE)
				{
					sprintf(Msg, ERR214);
					writeline(Msg);
					writeline(line);
					errsum++;
				}

				/* Check if at start of a new input section */
				if (*Tok[0] == '[')
				{
					newsect = findmatch(Tok[0], SectTxt);
					if (newsect >= 0)
					{
						sect = newsect;
						if (sect == _END) break;
						continue;
					}
					else
					{
						inperrmsg(201, sect, line);
						errsum++;
						break;
					}
				}

				/* Otherwise process next line of input in current section */
				else
				{
					inperr = newline(sect, line);
					if (inperr > 0)
					{
						inperrmsg(inperr, sect, line);
						errsum++;
					}
				}

				/* Stop if reach end of file or max. error count */
				if (errsum == MAXERRS) break;
			}   /* End of while */

				/* Check for errors */
			if (errsum > 0)  errcode = 200;
		}

		/* Check for unlinked nodes */
		if (!errcode) errcode = unlinked();

		/* Get pattern & curve data from temp. lists */
		if (!errcode) errcode = getpatterns();
		if (!errcode) errcode = getcurves();
		if (!errcode) errcode = getpumpparams();

		/* Free input buffer */
		free(X);
		return(errcode);

	}                        /*  End of readdata  */


	int  EPANET::newline(int sect, char *line)
		/*
		**--------------------------------------------------------------
		**  Input:   sect  = current section of input file
		**           *line = line read from input file
		**  Output:  returns error code or 0 if no error found
		**  Purpose: processes a new line of data from input file
		**--------------------------------------------------------------
		*/
	{
		int n;
		switch (sect)
		{
		case _TITLE:       if (Ntitle < 3)
		{
			n = strlen(line);
			if (line[n - 1] == 10) line[n - 1] = ' ';
			strncpy(Title[Ntitle], line, MAXMSG);
			Ntitle++;
		}
				   return(0);
		case _JUNCTIONS:   return(juncdata());
		case _RESERVOIRS:
		case _TANKS:       return(tankdata());
		case _PIPES:       return(pipedata());
		case _PUMPS:       return(pumpdata());
		case _VALVES:      return(valvedata());
		case _PATTERNS:    return(patterndata());
		case _CURVES:      return(curvedata());
		case _DEMANDS:     return(demanddata());
		case _CONTROLS:    return(controldata());
		case _RULES:       return(ruledata());   /* See RULES.C */
		case _SOURCES:     return(sourcedata());
		case _EMITTERS:    return(emitterdata());
		case _QUALITY:     return(qualdata());
		case _STATUS:      return(statusdata());
		case _ROUGHNESS:   return(0);
		case _ENERGY:      return(energydata());
		case _REACTIONS:   return(reactdata());
		case _MIXING:      return(mixingdata());
		case _REPORT:      return(reportdata());
		case _TIMES:       return(timedata());
		case _OPTIONS:     return(optiondata());

			/* Data in these sections are not used for any computations */
		case _COORDS:      return(0);
		case _LABELS:      return(0);
		case _TAGS:        return(0);
		case _VERTICES:    return(0);
		case _BACKDROP:    return(0);
		}
		return(201);
	}                        /* end of newline */


	int  EPANET::getpumpparams(void)
		/*
		**-------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: computes & checks pump curve parameters
		**--------------------------------------------------------------
		*/
	{
		int   i, j = 0, k, m, n = 0;
		double a, b, c,
			h0 = 0.0, h1 = 0.0, h2 = 0.0, q1 = 0.0, q2 = 0.0;

		for (i = 1; i <= Npumps; i++)
		{
			k = Pump[i].Link;
			if (Pump[i].Ptype == CONST_HP)      /* Constant Hp pump */
			{
				Pump[i].H0 = 0.0;
				Pump[i].R = -8.814 * Link[k].Km;
				Pump[i].N = -1.0;
				Pump[i].Hmax = BIG;             /* No head limit      */
				Pump[i].Qmax = BIG;             /* No flow limit      */
				Pump[i].Q0 = 1.0;                /* Init. flow = 1 cfs */
				continue;
			}

			/* Set parameters for pump curves */
			else if (Pump[i].Ptype == NOCURVE)  /* Pump curve specified */
			{
				j = Pump[i].Hcurve;              /* Get index of head curve */
				if (j == 0)
				{                                /* Error: No head curve */
					sprintf(Msg, ERR226, Link[k].ID);
					writeline(Msg);
					return(200);
				}
				n = Curve[j].Npts;
				if (n == 1)                      /* Only a single h-q point */
				{                                /* supplied so use generic */
					Pump[i].Ptype = POWER_FUNC;   /* power function curve.   */
					q1 = Curve[j].X[0];
					h1 = Curve[j].Y[0];
					h0 = 1.33334 * h1;
					q2 = 2.0 * q1;
					h2 = 0.0;
				}
				else if (n == 3
					&& Curve[j].X[0] == 0.0)   /* 3 h-q points supplied with */
				{                                /* shutoff head so use fitted */
					Pump[i].Ptype = POWER_FUNC;   /* power function curve.      */
					h0 = Curve[j].Y[0];
					q1 = Curve[j].X[1];
					h1 = Curve[j].Y[1];
					q2 = Curve[j].X[2];
					h2 = Curve[j].Y[2];
				}
				else Pump[i].Ptype = CUSTOM;     /* Else use custom pump curve.*/

												 /* Compute shape factors & limits of power function pump curves */
				if (Pump[i].Ptype == POWER_FUNC)
				{
					if (!powercurve(h0, h1, h2, q1, q2, &a, &b, &c))
					{                             /* Error: Invalid curve */
						sprintf(Msg, ERR227, Link[k].ID);
						writeline(Msg);
						return(200);
					}
					else
					{
						Pump[i].H0 = -a;
						Pump[i].R = -b;
						Pump[i].N = c;
						Pump[i].Q0 = q1;
						Pump[i].Qmax = pow((-a / b), (1.0 / c));
						Pump[i].Hmax = h0;
					}
				}
			}

			/* Assign limits to custom pump curves */
			if (Pump[i].Ptype == CUSTOM)
			{
				for (m = 1; m < n; m++)
				{
					if (Curve[j].Y[m] >= Curve[j].Y[m - 1])
					{                             /* Error: Invalid curve */
						sprintf(Msg, ERR227, Link[k].ID);
						writeline(Msg);
						return(200);
					}
				}
				Pump[i].Qmax = Curve[j].X[n - 1];
				Pump[i].Q0 = (Curve[j].X[0] + Pump[i].Qmax) / 2.0;
				Pump[i].Hmax = Curve[j].Y[0];
			}
		}   /* Next pump */
		return(0);
	}


	int   EPANET::addnodeID(int n, char *id)
		/*
		**-------------------------------------------------------------
		**  Input:   n = node index
		**           id = ID label
		**  Output:  returns 0 if ID already in use, 1 if not
		**  Purpose: adds a node ID to the Node Hash Table
		**--------------------------------------------------------------
		*/
	{
		if (findnode(id)) return(0);         /* see EPANET.C */
		strncpy(Node[n].ID, id, MAXID);
		HTinsert(Nht, Node[n].ID, n);        /* see HASH.C */
		return(1);
	}


	int   EPANET::addlinkID(int n, char *id)
		/*
		**-------------------------------------------------------------
		**  Input:   n = link index
		**           id = ID label
		**  Output:  returns 0 if ID already in use, 1 if not
		**  Purpose: adds a link ID to the Link Hash Table
		**--------------------------------------------------------------
		*/
	{
		if (findlink(id)) return(0);         /* see EPANET.C */
		strncpy(Link[n].ID, id, MAXID);
		HTinsert(Lht, Link[n].ID, n);        /* see HASH.C */
		return(1);
	}


	int  EPANET::addpattern(const char *id)
		/*
		**-------------------------------------------------------------
		**  Input:   id = pattern ID label
		**  Output:  returns error code
		**  Purpose: adds a new pattern to the database
		**--------------------------------------------------------------
		*/
	{
		STmplist *p;

		/* Check if ID is same as last one processed */
		if (Patlist != NULL && strcmp(id, Patlist->ID) == 0) return(0);

		/* Check that pattern was not already created */
		if (findID(id, Patlist) == NULL)
		{

			/* Update pattern count & create new list element */
			(MaxPats)++;
			p = (STmplist *)malloc(sizeof(STmplist));
			if (p == NULL) return(101);

			/* Initialize list element properties */
			else
			{
				p->i = MaxPats;
				strncpy(p->ID, id, MAXID);
				p->x = NULL;
				p->y = NULL;
				p->next = Patlist;
				Patlist = p;
			}
		}
		return(0);
	}


	int  EPANET::addcurve(char *id)
		/*
		**-------------------------------------------------------------
		**  Input:   id = curve ID label
		**  Output:  returns error code
		**  Purpose: adds a new curve to the database
		**--------------------------------------------------------------
		*/
	{
		STmplist *c;

		/* Check if ID is same as last one processed */
		if (Curvelist != NULL && strcmp(id, Curvelist->ID) == 0) return(0);

		/* Check that curve was not already created */
		if (findID(id, Curvelist) == NULL)
		{

			/* Update curve count & create new list element */
			(MaxCurves)++;
			c = (STmplist *)malloc(sizeof(STmplist));
			if (c == NULL) return(101);

			/* Initialize list element properties */
			else
			{
				c->i = MaxCurves;
				strncpy(c->ID, id, MAXID);
				c->x = NULL;
				c->y = NULL;
				c->next = Curvelist;
				Curvelist = c;
			}
		}
		return(0);
	}


	STmplist *EPANET::findID(const char *id, STmplist *list)
		/*
		**-------------------------------------------------------------
		**  Input:   id = ID label
		**           list = pointer to head of a temporary list
		**  Output:  returns list item with requested ID label
		**  Purpose: searches for item in temporary list
		**-------------------------------------------------------------
		*/
	{
		STmplist *item;
		for (item = list; item != NULL; item = item->next)
		{
			if (strcmp(item->ID, id) == 0)
			{
				return(item);
			}
		}
		return(NULL);
	}


	int  EPANET::unlinked()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  returns error code if any unlinked junctions found
		** Purpose: checks for unlinked junctions in network
		**
		** NOTE: unlinked tanks have no effect on computations.
		**--------------------------------------------------------------
		*/
	{
		char *marked;
		int   i, err, errcode;
		errcode = 0;
		err = 0;
		marked = (char *)calloc(Nnodes + 1, sizeof(char));
		ERRCODE(MEMCHECK(marked));
		if (!errcode)
		{
			memset(marked, 0, (Nnodes + 1) * sizeof(char));
			for (i = 1; i <= Nlinks; i++)            /* Mark end nodes of each link */
			{
				marked[Link[i].N1]++;
				marked[Link[i].N2]++;
			}
			for (i = 1; i <= Njuncs; i++)            /* Check each junction  */
			{
				if (marked[i] == 0)               /* If not marked then error */
				{
					err++;
					sprintf(Msg, ERR233, Node[i].ID);
					writeline(Msg);
				}
				if (err >= MAXERRS) break;
			}
			if (err > 0) errcode = 200;
		}
		free(marked);
		return(errcode);
	}                        /* End of unlinked */


	int     EPANET::getpatterns(void)
		/*
		**-----------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: retrieves pattern data from temporary linked list
		**-------------------------------------------------------------
		*/
	{
		int i, j;
		SFloatlist *f;
		STmplist *pat;

		/* Start at head of list */
		pat = Patlist;

		/* Traverse list of patterns */
		while (pat != NULL)
		{

			/* Get index of current pattern in Pattern array */
			i = pat->i;

			/* Check if this is the default pattern */
			if (strcmp(pat->ID, DefPatID) == 0) DefPat = i;
			if (i >= 0 && i <= MaxPats)
			{
				/* Save pattern ID */
				strcpy(Pattern[i].ID, pat->ID);

				/* Give pattern a length of at least 1 */
				if (Pattern[i].Length == 0) Pattern[i].Length = 1;
				Pattern[i].F = (double *)calloc(Pattern[i].Length, sizeof(double));
				if (Pattern[i].F == NULL) return(101);

				/* Start at head of pattern multiplier list */
				/* (which holds multipliers in reverse order)*/
				f = pat->x;
				j = Pattern[i].Length - 1;

				/* Use at least one multiplier equal to 1.0 */
				if (f == NULL) Pattern[i].F[0] = 1.0;

				/* Traverse list, storing multipliers in Pattern array */
				else while (f != NULL && j >= 0)
				{
					Pattern[i].F[j] = f->value;
					f = f->next;
					j--;
				}
			}
			pat = pat->next;
		}
		return(0);
	}


	int     EPANET::getcurves(void)
		/*
		**-----------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: retrieves curve data from temporary linked list
		**-----------------------------------------------------------
		*/
	{
		int i, j;
		double x;
		SFloatlist *fx, *fy;
		STmplist *c;

		/* Start at head of curve list */
		c = Curvelist;

		/* Traverse list of curves */
		while (c != NULL)
		{
			i = c->i;
			if (i >= 1 && i <= MaxCurves)
			{

				/* Save curve ID */
				strcpy(Curve[i].ID, c->ID);

				/* Check that curve has data points */
				if (Curve[i].Npts <= 0)
				{
					sprintf(Msg, ERR230, c->ID);
					writeline(Msg);
					return(200);
				}

				/* Allocate memory for curve data */
				Curve[i].X = (double *)calloc(Curve[i].Npts, sizeof(double));
				Curve[i].Y = (double *)calloc(Curve[i].Npts, sizeof(double));
				if (Curve[i].X == NULL || Curve[i].Y == NULL) return(101);

				/* Traverse list of x,y data */
				x = BIG;
				fx = c->x;
				fy = c->y;
				j = Curve[i].Npts - 1;
				while (fx != NULL && fy != NULL && j >= 0)
				{

					/* Check that x data is in ascending order */
					if (fx->value >= x)
					{
						sprintf(Msg, ERR230, c->ID);
						writeline(Msg);
						return(200);
					}
					x = fx->value;

					/* Save x,y data in Curve structure */
					Curve[i].X[j] = fx->value;
					fx = fx->next;
					Curve[i].Y[j] = fy->value;
					fy = fy->next;
					j--;
				}
			}
			c = c->next;
		}
		return(0);
	}


	int  EPANET::findmatch(const char *line,const char *keyword[])
		/*
		**--------------------------------------------------------------
		**  Input:   *line      = line from input file
		**           *keyword[] = list of NULL terminated keywords
		**  Output:  returns index of matching keyword or
		**           -1 if no match found
		**  Purpose: determines which keyword appears on input line
		**--------------------------------------------------------------
		*/
	{
		int i = 0;
		while (keyword[i] != NULL)
		{
			if (match(line, keyword[i])) return(i);
			i++;
		}
		return(-1);
	}                        /* end of findmatch */



	int  EPANET::match(const char *str,const char *substr)
		/*
		**--------------------------------------------------------------
		**  Input:   *str    = string being searched
		**           *substr = substring being searched for
		**  Output:  returns 1 if substr found in str, 0 if not
		**  Purpose: sees if substr matches any part of str
		**
		**      (Not case sensitive)
		**--------------------------------------------------------------
		*/
	{
		int i, j;

		/*** Updated 9/7/00 ***/
		/* Fail if substring is empty */
		if (!substr[0]) return(0);

		/* Skip leading blanks of str. */
		for (i = 0; str[i]; i++)
			if (str[i] != ' ') break;

		/* Check if substr matches remainder of str. */
		for (i = i, j = 0; substr[j]; i++, j++)
			if (!str[i] || UCHAR(str[i]) != UCHAR(substr[j]))
				return(0);
		return(1);
	}                        /* end of match */


							 /*** Updated 10/25/00 ***/
							 /* The gettokens function has been totally re-written. */

	int  EPANET::gettokens(char *s)
		/*
		**--------------------------------------------------------------
		**  Input:   *s = string to be tokenized
		**  Output:  returns number of tokens in s
		**  Purpose: scans string for tokens, saving pointers to them
		**           in module global variable Tok[]
		**
		** Tokens can be separated by the characters listed in SEPSTR
		** (spaces, tabs, newline, carriage return) which is defined
		** in TYPES.H. Text between quotes is treated as a single token.
		**--------------------------------------------------------------
		*/
	{
		int  len, m, n;
		char *c;

		/* Begin with no tokens */
		for (n = 0; n < MAXTOKS; n++) Tok[n] = NULL;
		n = 0;

		/* Truncate s at start of comment */
		c = strchr(s, ';');
		if (c) *c = '\0';
		len = strlen(s);

		/* Scan s for tokens until nothing left */
		while (len > 0 && n < MAXTOKS)
		{
			m = strcspn(s, SEPSTR);          /* Find token length */
			len -= m + 1;                     /* Update length of s */
			if (m == 0) s++;                /* No token found */
			else
			{
				if (*s == '"')               /* Token begins with quote */
				{
					s++;                      /* Start token after quote */
					m = strcspn(s, "\"\n\r");  /* Find end quote (or EOL) */
				}
				s[m] = '\0';                 /* Null-terminate the token */
				Tok[n] = s;                  /* Save pointer to token */
				n++;                         /* Update token count */
				s += m + 1;                    /* Begin next token */
			}
		}
		return(n);
	}                        /* End of gettokens */


	double  EPANET::hour(char *time,const char *units)
		/*
		**---------------------------------------------------------
		**  Input:   *time  = string containing a time value
		**           *units = string containing time units
		**  Output:  returns numerical value of time in hours,
		**           or -1 if an error occurs
		**  Purpose: converts time from units to hours
		**---------------------------------------------------------
		*/
	{
		int    n;
		double  y[3];
		char *s;

		/* Separate clock time into hrs, min, sec. */
		for (n = 0; n < 3; n++) y[n] = 0.0;
		n = 0;
		s = strtok(time, ":");
		while (s != NULL && n <= 3)
		{
			if (!getfloat(s, &y[n]))  return(-1.0);
			s = strtok(NULL, ":");
			n++;
		}

		/* If decimal time with units attached then convert to hours. */
		if (n == 1)
		{
			/*if (units[0] == '\0')       return(y[0]);*/
			if (strlen(units) == 0)     return(y[0]);
			if (match(units, w_SECONDS)) return(y[0] / 3600.0);
			if (match(units, w_MINUTES)) return(y[0] / 60.0);
			if (match(units, w_HOURS))   return(y[0]);
			if (match(units, w_DAYS))    return(y[0] * 24.0);
		}

		/* Convert hh:mm:ss format to decimal hours */
		if (n > 1) y[0] = y[0] + y[1] / 60.0 + y[2] / 3600.0;

		/* If am/pm attached then adjust hour accordingly */
		/* (12 am is midnight, 12 pm is noon) */
		if (units[0] == '\0')  return(y[0]);
		if (match(units, w_AM))
		{
			if (y[0] >= 13.0) return(-1.0);
			if (y[0] >= 12.0) return(y[0] - 12.0);
			else return(y[0]);
		}
		if (match(units, w_PM))
		{
			if (y[0] >= 13.0) return(-1.0);
			if (y[0] >= 12.0) return(y[0]);
			else return(y[0] + 12.0);
		}
		return(-1.0);
	}                        /* end of hour */


	int  EPANET::getfloat(char *s, double *y)
		/*
		**-----------------------------------------------------------
		**  Input:   *s = character string
		**  Output:  *y = floating point number
		**           returns 1 if conversion successful, 0 if not
		**  Purpose: converts string to floating point number
		**-----------------------------------------------------------
		*/
	{
		char *endptr;
		*y = (double)strtod(s, &endptr);
		if (*endptr > 0) return(0);
		return(1);
	}


	int  EPANET::setreport(char *s)
		/*
		**-----------------------------------------------------------
		**  Input:   *s = report format command
		**  Output:  none
		**  Returns: error code
		**  Purpose: processes a report formatting command
		**           issued by the ENsetreport function
		**-----------------------------------------------------------
		*/
	{
		Ntokens = gettokens(s);
		return(reportdata());
	}


	void  EPANET::inperrmsg(int err, int sect, char *line)
		/*
		**-------------------------------------------------------------
		**  Input:   err     = error code
		**           sect    = input data section
		**           *line   = line from input file
		**  Output:  none
		**  Purpose: displays input error message
		**-------------------------------------------------------------
		*/
	{
		char   fmt[MAXMSG + 1];
		char   id[MAXMSG + 1];

		/* Retrieve ID label of object with input error */
		/* (No ID used for CONTROLS or REPORT sections).*/
		if (sect == _CONTROLS || sect == _REPORT) strcpy(id, "");
		else if (sect == _ENERGY) strcpy(id, Tok[1]);
		else strcpy(id, Tok[0]);

		/* Copy error messge to string variable fmt */
		switch (err)
		{
		case 201:   strcpy(fmt, ERR201);  break;
		case 202:   strcpy(fmt, ERR202);  break;
		case 203:   strcpy(fmt, ERR203);  break;
		case 204:   strcpy(fmt, ERR204);  break;
		case 205:   strcpy(fmt, ERR205);  break;
		case 206:   strcpy(fmt, ERR206);  break;
		case 207:   strcpy(fmt, ERR207);  break;
		case 208:   strcpy(fmt, ERR208);  break;
		case 209:   strcpy(fmt, ERR209);  break;
		case 210:   strcpy(fmt, ERR210);  break;
		case 211:   strcpy(fmt, ERR211);  break;
		case 212:   strcpy(fmt, ERR212);  break;
		case 213:   strcpy(id, "");
			strcpy(fmt, ERR213);  break;
		case 214:   strcpy(id, "");
			strcpy(fmt, ERR214);  break;
		case 215:   strcpy(fmt, ERR215);  break;
		case 216:   strcpy(fmt, ERR216);  break;
		case 217:   strcpy(fmt, ERR217);  break;
		case 219:   strcpy(fmt, ERR219);  break;
		case 220:   strcpy(fmt, ERR220);  break;

			/*** Updated 10/25/00 ***/
		case 222:   strcpy(fmt, ERR222); break;

		default:    return;
		}

		/* Write error message to Report file */
		sprintf(Msg, fmt, RptSectTxt[sect], id);
		writeline(Msg);

		/* Echo input line for syntax errors, and   */
		/* errors in CONTROLS and OPTIONS sections. */
		if (sect == _CONTROLS || err == 201 || err == 213) writeline(line);
		else writeline("");
	}

	/*input3*/

	int EPANET::juncdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes junction data
		**  Format:
		**    [JUNCTIONS]
		**      id  elev.  (demand)  (demand pattern)
		**--------------------------------------------------------------
		*/
	{
		int      n, p = 0;
		double    el, y = 0.0;
		Pdemand  demand;
		STmplist *pat;

		/* Add new junction to data base */
		n = Ntokens;
		if (Nnodes == MaxNodes) return(200);
		Njuncs++;
		Nnodes++;
		if (!addnodeID(Njuncs, Tok[0])) return(215);

		/* Check for valid data */
		if (n < 2) return(201);
		if (!getfloat(Tok[1], &el)) return(202);
		if (n >= 3 && !getfloat(Tok[2], &y)) return(202);
		if (n >= 4)
		{
			pat = findID(Tok[3], Patlist);
			if (pat == NULL) return(205);
			p = pat->i;
		}

		/* Save junction data */
		Node[Njuncs].El = el;
		Node[Njuncs].C0 = 0.0;
		Node[Njuncs].S = NULL;
		Node[Njuncs].Ke = 0.0;
		Node[Njuncs].Rpt = 0;

		/* Create a new demand record */
		/*** Updated 6/24/02 ***/
		if (n >= 3)
		{
			demand = (Sdemand *)malloc(sizeof(Sdemand));
			if (demand == NULL) return(101);
			demand->Base = y;
			demand->Pat = p;
			demand->next = Node[Njuncs].D;
			Node[Njuncs].D = demand;
			D[Njuncs] = y;
		}
		else D[Njuncs] = MISSING;
		/*** end of update ***/
		return(0);
	}                        /* end of juncdata */


	int EPANET::tankdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes tank & reservoir data
		**  Format:
		**   [RESERVOIRS]
		**     id elev (pattern)
		**   [TANKS]
		**     id elev (pattern)
		**     id elev initlevel minlevel maxlevel diam (minvol vcurve)
		**--------------------------------------------------------------
		*/
	{
		int   i,               /* Node index */
			n,               /* # data items */
			p = 0,           /* Fixed grade time pattern index */
			vcurve = 0;      /* Volume curve index */
		double el = 0.0, /* Elevation */
			initlevel = 0.0, /* Initial level */
			minlevel = 0.0, /* Minimum level */
			maxlevel = 0.0, /* Maximum level */
			minvol = 0.0, /* Minimum volume */
			diam = 0.0, /* Diameter */
			area;            /* X-sect. area */
		STmplist *t;

		/* Add new tank to data base */
		n = Ntokens;
		if (Ntanks == MaxTanks
			|| Nnodes == MaxNodes) return(200);
		Ntanks++;
		Nnodes++;
		i = MaxJuncs + Ntanks;                    /* i = node index.     */
		if (!addnodeID(i, Tok[0])) return(215);    /* Add ID to database. */

												   /* Check for valid data */
		if (n < 2) return(201);                   /* Too few fields.   */
		if (!getfloat(Tok[1], &el)) return(202);   /* Read elevation    */
		if (n <= 3)                               /* Tank is reservoir.*/
		{
			if (n == 3)                            /* Pattern supplied  */
			{
				t = findID(Tok[2], Patlist);
				if (t == NULL) return(205);
				p = t->i;
			}
		}
		else if (n < 6) return(201);              /* Too few fields for tank.*/
		else
		{
			/* Check for valid input data */
			if (!getfloat(Tok[2], &initlevel)) return(202);
			if (!getfloat(Tok[3], &minlevel))  return(202);
			if (!getfloat(Tok[4], &maxlevel))  return(202);
			if (!getfloat(Tok[5], &diam))      return(202);
			if (diam < 0.0)                   return(202);
			if (n >= 7
				&& !getfloat(Tok[6], &minvol))     return(202);

			/* If volume curve supplied check it exists */
			if (n == 8)
			{
				t = findID(Tok[7], Curvelist);
				if (t == NULL) return(202);
				vcurve = t->i;
			}
		}

		Node[i].Rpt = 0;
		Node[i].El = el;               /* Elevation.           */
		Node[i].C0 = 0.0;              /* Init. quality.       */
		Node[i].S = NULL;             /* WQ source data       */
		Node[i].Ke = 0.0;              /* Emitter coeff.       */
		Tank[Ntanks].Node = i;                /* Node index.          */
		Tank[Ntanks].H0 = initlevel;        /* Init. level.         */
		Tank[Ntanks].Hmin = minlevel;         /* Min. level.          */
		Tank[Ntanks].Hmax = maxlevel;         /* Max level.           */
		Tank[Ntanks].A = diam;             /* Diameter.            */
		Tank[Ntanks].Pat = p;                /* Fixed grade pattern. */
		Tank[Ntanks].Kb = MISSING;          /* Reaction coeff.      */
											/*
											*******************************************************************
											NOTE: The min, max, & initial volumes set here are based on a
											nominal tank diameter. They will be modified in INPUT1.C if
											a volume curve is supplied for this tank.
											*******************************************************************
											*/
		area = PI * SQR(diam) / 4.0;
		Tank[Ntanks].Vmin = area * minlevel;
		if (minvol > 0.0) Tank[Ntanks].Vmin = minvol;
		Tank[Ntanks].V0 = Tank[Ntanks].Vmin + area * (initlevel - minlevel);
		Tank[Ntanks].Vmax = Tank[Ntanks].Vmin + area * (maxlevel - minlevel);

		Tank[Ntanks].Vcurve = vcurve;           /* Volume curve         */
		Tank[Ntanks].MixModel = MIX1;             /* Completely mixed     */
		Tank[Ntanks].V1max = 1.0;              /* Compart. size ratio  */
		return(0);
	}                        /* end of tankdata */


	int EPANET::pipedata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes pipe data
		**  Format:
		**    [PIPE]
		**    id  node1  node2  length  diam  rcoeff (lcoeff) (status)
		**--------------------------------------------------------------
		*/
	{
		int   j1,                     /* Start-node index  */
			j2,                     /* End-node index    */
			n;                      /* # data items      */
		char  type = PIPE,            /* Link type         */
			status = OPEN;          /* Link status       */
		double length,                 /* Link length       */
			diam,                   /* Link diameter     */
			rcoeff,                 /* Roughness coeff.  */
			lcoeff = 0.0;           /* Minor loss coeff. */

									/* Add new pipe to data base */
		n = Ntokens;
		if (Nlinks == MaxLinks) return(200);
		Npipes++;
		Nlinks++;
		if (!addlinkID(Nlinks, Tok[0])) return(215);

		/* Check for valid data */
		if (n < 6) return(201);
		if ((j1 = findnode(Tok[1])) == 0 ||
			(j2 = findnode(Tok[2])) == 0
			) return(203);

		/*** Updated 10/25/00 ***/
		if (j1 == j2) return(222);

		if (!getfloat(Tok[3], &length) ||
			!getfloat(Tok[4], &diam) ||
			!getfloat(Tok[5], &rcoeff)
			) return(202);

		if (length <= 0.0 ||
			diam <= 0.0 ||
			rcoeff <= 0.0
			) return(202);

		/* Case where either loss coeff. or status supplied */
		if (n == 7)
		{
			if (match(Tok[6], w_CV))        type = CV;
			else if (match(Tok[6], w_CLOSED))    status = CLOSED;
			else if (match(Tok[6], w_OPEN))      status = OPEN;
			else if (!getfloat(Tok[6], &lcoeff)) return(202);
		}

		/* Case where both loss coeff. and status supplied */
		if (n == 8)
		{
			if (!getfloat(Tok[6], &lcoeff))   return(202);
			if (match(Tok[7], w_CV))     type = CV;
			else if (match(Tok[7], w_CLOSED)) status = CLOSED;
			else if (match(Tok[7], w_OPEN))   status = OPEN;
			else return(202);
		}
		if (lcoeff < 0.0) return(202);

		/* Save pipe data */
		Link[Nlinks].N1 = j1;                  /* Start-node index */
		Link[Nlinks].N2 = j2;                  /* End-node index   */
		Link[Nlinks].Len = length;              /* Length           */
		Link[Nlinks].Diam = diam;                /* Diameter         */
		Link[Nlinks].Kc = rcoeff;              /* Rough. coeff     */
		Link[Nlinks].Km = lcoeff;              /* Loss coeff       */
		Link[Nlinks].Kb = MISSING;             /* Bulk coeff       */
		Link[Nlinks].Kw = MISSING;             /* Wall coeff       */
		Link[Nlinks].Type = type;                /* Link type        */
		Link[Nlinks].Stat = status;              /* Link status      */
		Link[Nlinks].Rpt = 0;                   /* Report flag      */
		return(0);
	}                        /* end of pipedata */


	int  EPANET::pumpdata()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  returns error code
		** Purpose: processes pump data
		** Formats:
		**  [PUMP]
		**   (Version 1.x Format):
		**   id  node1  node2  power
		**   id  node1  node2  h1    q1
		**   id  node1  node2  h0    h1   q1   h2   q2
		**   (Version 2 Format):
		**   id  node1  node2  KEYWORD value {KEYWORD value ...}
		**   where KEYWORD = [POWER,HEAD,PATTERN,SPEED]
		**--------------------------------------------------------------
		*/
	{
		int   j,
			j1,                    /* Start-node index */
			j2,                    /* End-node index   */
			m, n;                  /* # data items     */
		double y;
		STmplist *t;                 /* Pattern record   */

									 /* Add new pump to data base */
		n = Ntokens;
		if (Nlinks == MaxLinks ||
			Npumps == MaxPumps
			) return(200);
		Nlinks++;
		Npumps++;
		if (!addlinkID(Nlinks, Tok[0])) return(215);

		/* Check for valid data */
		if (n < 4) return(201);
		if ((j1 = findnode(Tok[1])) == 0 ||
			(j2 = findnode(Tok[2])) == 0
			) return(203);

		/*** Updated 10/25/00 ***/
		if (j1 == j2) return(222);

		/* Save pump data */
		Link[Nlinks].N1 = j1;               /* Start-node index.  */
		Link[Nlinks].N2 = j2;               /* End-node index.    */
		Link[Nlinks].Diam = Npumps;           /* Pump index.        */
		Link[Nlinks].Len = 0.0;              /* Link length.       */
		Link[Nlinks].Kc = 1.0;              /* Speed factor.      */
		Link[Nlinks].Km = 0.0;              /* Horsepower.        */
		Link[Nlinks].Kb = 0.0;
		Link[Nlinks].Kw = 0.0;
		Link[Nlinks].Type = PUMP;             /* Link type.         */
		Link[Nlinks].Stat = OPEN;             /* Link status.       */
		Link[Nlinks].Rpt = 0;                /* Report flag.       */
		Pump[Npumps].Link = Nlinks;            /* Link index.        */
		Pump[Npumps].Ptype = NOCURVE;          /* Type of pump curve */
		Pump[Npumps].Hcurve = 0;               /* Pump curve index   */
		Pump[Npumps].Ecurve = 0;               /* Effic. curve index */
		Pump[Npumps].Upat = 0;               /* Utilization pattern*/
		Pump[Npumps].Ecost = 0.0;             /* Unit energy cost   */
		Pump[Npumps].Epat = 0;               /* Energy cost pattern*/

											 /* If 4-th token is a number then input follows Version 1.x format */
											 /* so retrieve pump curve parameters */
		if (getfloat(Tok[3], &X[0]))
		{
			m = 1;
			for (j = 4; j < n; j++)
			{
				if (!getfloat(Tok[j], &X[m])) return(202);
				m++;
			}
			return(getpumpcurve(m));          /* Get pump curve params */
		}

		/* Otherwise input follows Version 2 format */
		/* so retrieve keyword/value pairs.         */
		m = 4;
		while (m < n)
		{
			if (match(Tok[m - 1], w_POWER))          /* Const. HP curve       */
			{
				y = atof(Tok[m]);
				if (y <= 0.0) return(202);
				Pump[Npumps].Ptype = CONST_HP;
				Link[Nlinks].Km = y;
			}
			else if (match(Tok[m - 1], w_HEAD))      /* Custom pump curve      */
			{
				t = findID(Tok[m], Curvelist);
				if (t == NULL) return(206);
				Pump[Npumps].Hcurve = t->i;
			}
			else if (match(Tok[m - 1], w_PATTERN))   /* Speed/status pattern */
			{
				t = findID(Tok[m], Patlist);
				if (t == NULL) return(205);
				Pump[Npumps].Upat = t->i;
			}
			else if (match(Tok[m - 1], w_SPEED))     /* Speed setting */
			{
				if (!getfloat(Tok[m], &y)) return(202);
				if (y < 0.0) return(202);
				Link[Nlinks].Kc = y;
			}
			else return(201);
			m = m + 2;                          /* Skip to next keyword token */
		}
		return(0);
	}                        /* end of pumpdata */


	int  EPANET::valvedata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes valve data
		**  Format:
		**     [VALVE]
		**        id  node1  node2  diam  type  setting (lcoeff)
		**--------------------------------------------------------------
		*/
	{
		int   j1,                    /* Start-node index   */
			j2,                    /* End-node index     */
			n;                     /* # data items       */
		char  status = ACTIVE,       /* Valve status       */
			type;                  /* Valve type         */
		double diam = 0.0,            /* Valve diameter     */
			setting,               /* Valve setting      */
			lcoeff = 0.0;          /* Minor loss coeff.  */
		STmplist *t;                 /* Curve record       */

									 /* Add new valve to data base */
		n = Ntokens;
		if (Nlinks == MaxLinks ||
			Nvalves == MaxValves
			) return(200);
		Nvalves++;
		Nlinks++;
		if (!addlinkID(Nlinks, Tok[0])) return(215);

		/* Check for valid data */
		if (n < 6) return(201);
		if ((j1 = findnode(Tok[1])) == 0 ||
			(j2 = findnode(Tok[2])) == 0
			) return(203);

		/*** Updated 10/25/00 ***/
		if (j1 == j2) return(222);

		if (match(Tok[4], w_PRV)) type = PRV;
		else if (match(Tok[4], w_PSV)) type = PSV;
		else if (match(Tok[4], w_PBV)) type = PBV;
		else if (match(Tok[4], w_FCV)) type = FCV;
		else if (match(Tok[4], w_TCV)) type = TCV;
		else if (match(Tok[4], w_GPV)) type = GPV;
		else    return(201);                      /* Illegal valve type.*/
		if (!getfloat(Tok[3], &diam)) return(202);
		if (diam <= 0.0) return(202);             /* Illegal diameter.*/
		if (type == GPV)                          /* Headloss curve for GPV */
		{
			t = findID(Tok[5], Curvelist);
			if (t == NULL) return(206);
			setting = t->i;

			/*** Updated 9/7/00 ***/
			status = OPEN;

		}
		else if (!getfloat(Tok[5], &setting)) return(202);
		if (n >= 7 &&
			!getfloat(Tok[6], &lcoeff)
			) return(202);

		/* Check that PRV, PSV, or FCV not connected to a tank & */
		/* check for illegal connections between pairs of valves.*/
		if ((j1 > Njuncs || j2 > Njuncs) &&
			(type == PRV || type == PSV || type == FCV)
			) return(219);
		if (!valvecheck(type, j1, j2)) return(220);

		/* Save valve data */
		Link[Nlinks].N1 = j1;                 /* Start-node index. */
		Link[Nlinks].N2 = j2;                 /* End-node index.   */
		Link[Nlinks].Diam = diam;               /* Valve diameter.   */
		Link[Nlinks].Len = 0.0;                /* Link length.      */
		Link[Nlinks].Kc = setting;            /* Valve setting.    */
		Link[Nlinks].Km = lcoeff;             /* Loss coeff        */
		Link[Nlinks].Kb = 0.0;
		Link[Nlinks].Kw = 0.0;
		Link[Nlinks].Type = type;               /* Valve type.       */
		Link[Nlinks].Stat = status;             /* Valve status.     */
		Link[Nlinks].Rpt = 0;                  /* Report flag.      */
		Valve[Nvalves].Link = Nlinks;             /* Link index.       */
		return(0);
	}                        /* end of valvedata */


	int  EPANET::patterndata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes time pattern data
		**  Format:
		**     [PATTERNS]
		**        id  mult1  mult2 .....
		**--------------------------------------------------------------
		*/
	{
		int  i, n;
		double x;
		SFloatlist *f;
		STmplist *p;
		n = Ntokens - 1;
		if (n < 1) return(201);            /* Too few values        */
		if (                               /* Check for new pattern */
			PrevPat != NULL &&
			strcmp(Tok[0], PrevPat->ID) == 0
			) p = PrevPat;
		else p = findID(Tok[0], Patlist);
		if (p == NULL) return(205);
		for (i = 1; i <= n; i++)               /* Add multipliers to list */
		{
			if (!getfloat(Tok[i], &x)) return(202);
			f = (SFloatlist *)malloc(sizeof(SFloatlist));
			if (f == NULL) return(101);
			f->value = x;
			f->next = p->x;
			p->x = f;
		}
		Pattern[p->i].Length += n;         /* Save # multipliers for pattern */
		PrevPat = p;                       /* Set previous pattern pointer */
		return(0);
	}                        /* end of patterndata */


	int  EPANET::curvedata()
		/*
		**------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes curve data
		**  Format:
		**     [CURVES]
		**      CurveID   x-value  y-value
		**------------------------------------------------------
		*/
	{
		double      x, y;
		SFloatlist *fx, *fy;
		STmplist *c;

		/* Check for valid curve ID */
		if (Ntokens < 3) return(201);
		if (
			PrevCurve != NULL &&
			strcmp(Tok[0], PrevCurve->ID) == 0
			) c = PrevCurve;
		else c = findID(Tok[0], Curvelist);
		if (c == NULL) return(205);

		/* Check for valid data */
		if (!getfloat(Tok[1], &x)) return(202);
		if (!getfloat(Tok[2], &y)) return(202);

		/* Add new data point to curve's linked list */
		fx = (SFloatlist *)malloc(sizeof(SFloatlist));
		fy = (SFloatlist *)malloc(sizeof(SFloatlist));
		if (fx == NULL || fy == NULL) return(101);
		fx->value = x;
		fx->next = c->x;
		c->x = fx;
		fy->value = y;
		fy->next = c->y;
		c->y = fy;
		Curve[c->i].Npts++;

		/* Save the pointer to this curve */
		PrevCurve = c;
		return(0);
	}


	int EPANET::demanddata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes node demand data
		**  Format:
		**     [DEMANDS]
		**        MULTIPLY  factor
		**        node  base_demand  (pattern)
		**
		**  NOTE: Demands entered in this section replace those
		**        entered in the [JUNCTIONS] section
		**--------------------------------------------------------------
		*/
	{
		int  j, n, p = 0;
		double y;
		Pdemand demand;
		STmplist *pat;

		/* Extract data from tokens */
		n = Ntokens;
		if (n < 2) return(201);
		if (!getfloat(Tok[1], &y)) return(202);

		/* If MULTIPLY command, save multiplier */
		if (match(Tok[0], w_MULTIPLY))
		{
			if (y <= 0.0) return(202);
			else Dmult = y;
			return(0);
		}

		/* Otherwise find node (and pattern) being referenced */
		if ((j = findnode(Tok[0])) == 0) return(208);
		if (j > Njuncs) return(208);
		if (n >= 3)
		{
			pat = findID(Tok[2], Patlist);
			if (pat == NULL)  return(205);
			p = pat->i;
		}

		/* Replace any demand entered in [JUNCTIONS] section */
		/* (Such demand was temporarily stored in D[]) */

		/*** Updated 6/24/02 ***/
		demand = Node[j].D;
		if (demand && D[j] != MISSING)
		{
			demand->Base = y;
			demand->Pat = p;
			D[j] = MISSING;
		}
		/*** End of update ***/

		/* Otherwise add a new demand to this junction */
		else
		{
			demand = (Sdemand *)malloc(sizeof(Sdemand));
			if (demand == NULL) return(101);
			demand->Base = y;
			demand->Pat = p;
			demand->next = Node[j].D;
			Node[j].D = demand;
		}
		return(0);
	}                        /* end of demanddata */


	int  EPANET::controldata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes simple controls
		**  Formats:
		**  [CONTROLS]
		**  LINK  linkID  setting IF NODE      nodeID {BELOW/ABOVE}  level
		**  LINK  linkID  setting AT TIME      value  (units)
		**  LINK  linkID  setting AT CLOCKTIME value  (units)
		**   (0)   (1)      (2)   (3) (4)       (5)     (6)          (7)
		**--------------------------------------------------------------
		*/
	{
		int   i = 0,                /* Node index             */
			k,                    /* Link index             */
			n;                    /* # data items           */
		char  status = ACTIVE,      /* Link status            */
			type;                 /* Link or control type   */
		double setting = MISSING,    /* Link setting           */
			time = 0.0,           /* Simulation time        */
			level = 0.0;          /* Pressure or tank level */

								  /* Check for sufficient number of input tokens */
		n = Ntokens;
		if (n < 6) return(201);

		/* Check that controlled link exists */
		k = findlink(Tok[1]);
		if (k == 0) return(204);
		type = Link[k].Type;
		if (type == CV) return(207);         /* Cannot control check valve. */

											 /*** Updated 9/7/00 ***/
											 /* Parse control setting into a status level or numerical setting. */
		if (match(Tok[2], w_OPEN))
		{
			status = OPEN;
			if (type == PUMP) setting = 1.0;
			if (type == GPV)  setting = Link[k].Kc;
		}
		else if (match(Tok[2], w_CLOSED))
		{
			status = CLOSED;
			if (type == PUMP) setting = 0.0;
			if (type == GPV)  setting = Link[k].Kc;
		}
		else if (type == GPV) return(206);
		else if (!getfloat(Tok[2], &setting)) return(202);

		/*** Updated 3/1/01 ***/
		/* Set status for pump in case speed setting was supplied */
		/* or for pipe if numerical setting was supplied */

		if (type == PUMP || type == PIPE)
		{
			if (setting != MISSING)
			{
				if (setting < 0.0)       return(202);
				else if (setting == 0.0) status = CLOSED;
				else                     status = OPEN;
			}
		}

		/* Determine type of control */
		if (match(Tok[4], w_TIME))      type = TIMER;
		else if (match(Tok[4], w_CLOCKTIME)) type = TIMEOFDAY;
		else
		{
			if (n < 8) return(201);
			if ((i = findnode(Tok[5])) == 0) return(203);
			if (match(Tok[6], w_BELOW)) type = LOWLEVEL;
			else if (match(Tok[6], w_ABOVE)) type = HILEVEL;
			else return(201);
		}

		/* Parse control level or time */
		switch (type)
		{
		case TIMER:
		case TIMEOFDAY:
			if (n == 6) time = hour(Tok[5], "");
			if (n == 7) time = hour(Tok[5], Tok[6]);
			if (time < 0.0) return(201);
			break;
		case LOWLEVEL:
		case HILEVEL:
			if (!getfloat(Tok[7], &level)) return(202);
			break;
		}

		/* Fill in fields of control data structure */
		Ncontrols++;
		if (Ncontrols > MaxControls) return(200);
		Control[Ncontrols].Link = k;
		Control[Ncontrols].Node = i;
		Control[Ncontrols].Type = type;
		Control[Ncontrols].Status = status;
		Control[Ncontrols].Setting = setting;
		Control[Ncontrols].Time = (long)(3600.0 * time);
		if (type == TIMEOFDAY)
			Control[Ncontrols].Time %= SECperDAY;
		Control[Ncontrols].Grade = level;
		return(0);
	}                        /* end of controldata */


	int EPANET::sourcedata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes water quality source data
		**  Formats:
		**     [SOURCE]
		**        node  sourcetype  quality  (pattern start stop)
		**
		**  NOTE: units of mass-based source are mass/min
		**--------------------------------------------------------------
		*/
	{
		int   i,                  /* Token with quality value */
			j,                  /* Node index    */
			n,                  /* # data items  */
			p = 0;              /* Time pattern  */
		char  type = CONCEN;      /* Source type   */
		double c0 = 0;             /* Init. quality */
		STmplist *pat;
		Psource  source;

		n = Ntokens;
		if (n < 2) return(201);
		if ((j = findnode(Tok[0])) == 0) return(203);
		/* NOTE: Under old format, SourceType not supplied so let  */
		/*       i = index of token that contains quality value.   */
		i = 2;
		if (match(Tok[1], w_CONCEN))    type = CONCEN;
		else if (match(Tok[1], w_MASS))      type = MASS;
		else if (match(Tok[1], w_SETPOINT))  type = SETPOINT;
		else if (match(Tok[1], w_FLOWPACED)) type = FLOWPACED;
		else i = 1;
		if (!getfloat(Tok[i], &c0)) return(202);      /* Illegal WQ value */

		if (n > i + 1 && strlen(Tok[i + 1]) > 0 && strcmp(Tok[i + 1], "*") != 0)         //(2.00.11 - LR)
		{
			pat = findID(Tok[i + 1], Patlist);
			if (pat == NULL) return(205);            /* Illegal pattern. */
			p = pat->i;
		}

		source = (Ssource *)malloc(sizeof(Ssource));
		if (source == NULL) return(101);
		source->C0 = c0;
		source->Pat = p;
		source->Type = type;
		Node[j].S = source;
		return(0);
	}                        /* end of sourcedata */


	int  EPANET::emitterdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes junction emitter data
		**  Format:
		**     [EMITTER]
		**        node   K
		**--------------------------------------------------------------
		*/
	{
		int   j,                  /* Node index    */
			n;                  /* # data items  */
		double k;                  /* Flow coeff,   */

		n = Ntokens;
		if (n < 2) return(201);
		if ((j = findnode(Tok[0])) == 0) return(203);
		if (j > Njuncs) return(209);                 /* Not a junction.*/
		if (!getfloat(Tok[1], &k)) return(202);
		if (k < 0.0) return(202);
		Node[j].Ke = k;
		return(0);
	}


	int  EPANET::qualdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes initial water quality data
		**  Formats:
		**     [QUALITY]
		**        node   initqual
		**        node1  node2    initqual
		**--------------------------------------------------------------
		*/
	{
		int   j, n;
		long  i, i0, i1;
		double c0;

		if (Nnodes == 0) return(208);        /* No nodes defined yet */
		n = Ntokens;
		if (n < 2) return(0);
		if (n == 2)                          /* Single node entered  */
		{
			if ((j = findnode(Tok[0])) == 0) return(0);
			if (!getfloat(Tok[1], &c0)) return(209);
			Node[j].C0 = c0;
		}
		else                                 /* Node range entered    */
		{
			if (!getfloat(Tok[2], &c0)) return(209);

			/* If numerical range supplied, then use numerical comparison */
			if ((i0 = atol(Tok[0])) > 0 && (i1 = atol(Tok[1])) > 0)
			{
				for (j = 1; j <= Nnodes; j++)
				{
					i = atol(Node[j].ID);
					if (i >= i0 && i <= i1) Node[j].C0 = c0;
				}
			}
			else
			{
				for (j = 1; j <= Nnodes; j++)
					if ((strcmp(Tok[0], Node[j].ID) <= 0) &&
						(strcmp(Tok[1], Node[j].ID) >= 0)
						) Node[j].C0 = c0;
			}
		}
		return(0);
	}                        /* end of qualdata */


	int  EPANET::reactdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes reaction coeff. data
		**  Formats:
		**     [REACTIONS]
		**        ORDER     {BULK/WALL/TANK} value
		**        GLOBAL    BULK             coeff
		**        GLOBAL    WALL             coeff
		**        BULK      link1  (link2)   coeff
		**        WALL      link1  (link2)   coeff
		**        TANK      node1  (node2)   coeff
		**        LIMITING  POTENTIAL        value
		**        ROUGHNESS CORRELATION      value
		**--------------------------------------------------------------
		*/
	{
		int   item, j, n;
		long  i, i1, i2;
		double y;

		/* Skip line if insufficient data */
		n = Ntokens;
		if (n < 3) return(0);

		/* Process input depending on keyword */
		if (match(Tok[0], w_ORDER))                    /* Reaction order */
		{
			if (!getfloat(Tok[n - 1], &y)) return(213);
			if (match(Tok[1], w_BULK)) BulkOrder = y;
			else if (match(Tok[1], w_TANK)) TankOrder = y;
			else if (match(Tok[1], w_WALL))
			{
				if (y == 0.0) WallOrder = 0.0;
				else if (y == 1.0) WallOrder = 1.0;
				else return(213);
			}
			else return(213);
			return(0);
		}
		if (match(Tok[0], w_ROUGHNESS))                /* Roughness factor */
		{
			if (!getfloat(Tok[n - 1], &y)) return(213);
			Rfactor = y;
			return(0);
		}
		if (match(Tok[0], w_LIMITING))                 /* Limiting potential */
		{
			if (!getfloat(Tok[n - 1], &y)) return(213);
			/*if (y < 0.0) return(213);*/
			Climit = y;
			return(0);
		}
		if (match(Tok[0], w_GLOBAL))                   /* Global rates */
		{
			if (!getfloat(Tok[n - 1], &y)) return(213);
			if (match(Tok[1], w_BULK)) Kbulk = y;
			else if (match(Tok[1], w_WALL)) Kwall = y;
			else return(201);
			return(0);
		}
		if (match(Tok[0], w_BULK)) item = 1;      /* Solution rates */
		else if (match(Tok[0], w_WALL)) item = 2;
		else if (match(Tok[0], w_TANK)) item = 3;
		else return(201);
		strcpy(Tok[0], Tok[1]);                        /* Save id in Tok[0] */
		if (item == 3)                                /* Tank rates */
		{
			if (!getfloat(Tok[n - 1], &y)) return(209);   /* Rate coeff. */
			if (n == 3)
			{
				if ((j = findnode(Tok[1])) <= Njuncs) return(0);
				Tank[j - Njuncs].Kb = y;
			}
			else
			{
				/* If numerical range supplied, then use numerical comparison */
				if ((i1 = atol(Tok[1])) > 0 && (i2 = atol(Tok[2])) > 0)
				{
					for (j = Njuncs + 1; j <= Nnodes; j++)
					{
						i = atol(Node[j].ID);
						if (i >= i1 && i <= i2) Tank[j - Njuncs].Kb = y;
					}
				}
				else for (j = Njuncs + 1; j <= Nnodes; j++)
					if ((strcmp(Tok[1], Node[j].ID) <= 0) &&
						(strcmp(Tok[2], Node[j].ID) >= 0)
						) Tank[j - Njuncs].Kb = y;
			}
		}
		else                                          /* Link rates */
		{
			if (!getfloat(Tok[n - 1], &y)) return(211);   /* Rate coeff. */
			if (Nlinks == 0) return(0);
			if (n == 3)                                /* Single link */
			{
				if ((j = findlink(Tok[1])) == 0) return(0);
				if (item == 1) Link[j].Kb = y;
				else           Link[j].Kw = y;
			}
			else                                       /* Range of links */
			{
				/* If numerical range supplied, then use numerical comparison */
				if ((i1 = atol(Tok[1])) > 0 && (i2 = atol(Tok[2])) > 0)
				{
					for (j = 1; j <= Nlinks; j++)
					{
						i = atol(Link[j].ID);
						if (i >= i1 && i <= i2)
						{
							if (item == 1) Link[j].Kb = y;
							else           Link[j].Kw = y;
						}
					}
				}
				else for (j = 1; j <= Nlinks; j++)
					if ((strcmp(Tok[1], Link[j].ID) <= 0) &&
						(strcmp(Tok[2], Link[j].ID) >= 0))
					{
						if (item == 1) Link[j].Kb = y;
						else           Link[j].Kw = y;
					}
			}
		}
		return(0);
	}                        /* end of reactdata */


	int  EPANET::mixingdata()
		/*
		**-------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes tank mixing data
		**  Format:
		**    [MIXING]
		**     TankID  MixModel  FractVolume
		**-------------------------------------------------------------
		*/
	{
		int   i, j, n;
		double v;

		if (Nnodes == 0) return(208);        /* No nodes defined yet */
		n = Ntokens;
		if (n < 2) return(0);
		if ((j = findnode(Tok[0])) <= Njuncs) return(0);
		if ((i = findmatch(Tok[1], MixTxt)) < 0) return(201);
		v = 1.0;
		if ((i == MIX2) &&
			(n == 3) &&
			(!getfloat(Tok[2], &v))          /* Get frac. vol. for 2COMP model */
			) return(209);
		if (v == 0.0) v = 1.0;               /* v can't be zero */
		n = j - Njuncs;
		if (Tank[n].A == 0.0) return(0);     /* Tank is a reservoir */
		Tank[n].MixModel = (char)i;
		Tank[n].V1max = v;
		return(0);
	}


	int  EPANET::statusdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes link initial status data
		**  Formats:
		**    [STATUS]
		**       link   value
		**       link1  (link2)  value
		**--------------------------------------------------------------
		*/
	{
		int   j, n;
		long  i, i0, i1;
		double y = 0.0;
		char  status = ACTIVE;

		if (Nlinks == 0) return(210);
		n = Ntokens - 1;
		if (n < 1) return(201);

		/* Check for legal status setting */
		if (match(Tok[n], w_OPEN))    status = OPEN;
		else if (match(Tok[n], w_CLOSED))  status = CLOSED;
		else if (!getfloat(Tok[n], &y))    return(211);
		if (y < 0.0)                      return(211);

		/* Single link ID supplied */
		if (n == 1)
		{
			if ((j = findlink(Tok[0])) == 0) return(0);
			/* Cannot change status of a Check Valve */
			if (Link[j].Type == CV) return(211);

			/*** Updated 9/7/00 ***/
			/* Cannot change setting for a GPV */
			if (Link[j].Type == GPV
				&& status == ACTIVE)   return(211);

			changestatus(j, status, y);
		}

		/* Range of ID's supplied */
		else
		{
			/* Numerical range supplied */
			if ((i0 = atol(Tok[0])) > 0 && (i1 = atol(Tok[1])) > 0)
			{
				for (j = 1; j <= Nlinks; j++)
				{
					i = atol(Link[j].ID);
					if (i >= i0 && i <= i1) changestatus(j, status, y);
				}
			}
			else
				for (j = 1; j <= Nlinks; j++)
					if ((strcmp(Tok[0], Link[j].ID) <= 0) &&
						(strcmp(Tok[1], Link[j].ID) >= 0)
						) changestatus(j, status, y);
		}
		return(0);
	}              /* end of statusdata */


	int  EPANET::energydata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes pump energy data
		**  Formats:
		**    [ENERGY]
		**       GLOBAL         {PRICE/PATTERN/EFFIC}  value
		**       PUMP   id      {PRICE/PATTERN/EFFIC}  value
		**       DEMAND CHARGE  value
		**--------------------------------------------------------------
		*/
	{
		int j, k, n;
		double y;
		STmplist *t;

		/* Check for sufficient data */
		n = Ntokens;
		if (n < 3) return(201);

		/* Check first keyword */
		if (match(Tok[0], w_DMNDCHARGE))               /* Demand charge */
		{
			if (!getfloat(Tok[2], &y)) return(213);
			Dcost = y;
			return(0);
		}
		if (match(Tok[0], w_GLOBAL))                   /* Global parameter */
		{
			j = 0;
		}
		else if (match(Tok[0], w_PUMP))                /* Pump-specific parameter */
		{
			if (n < 4) return(201);
			k = findlink(Tok[1]);                      /* Check that pump exists */
			if (k == 0) return(216);
			if (Link[k].Type != PUMP) return(216);
			j = PUMPINDEX(k);
		}
		else return(201);

		/* Find type of energy parameter */
		if (match(Tok[n - 2], w_PRICE))                  /* Energy price */
		{
			if (!getfloat(Tok[n - 1], &y))
			{
				if (j == 0) return(213);
				else return(217);
			}
			if (j == 0) Ecost = y;
			else Pump[j].Ecost = y;
			return(0);
		}
		else if (match(Tok[n - 2], w_PATTERN))           /* Price pattern */
		{
			t = findID(Tok[n - 1], Patlist);              /* Check if pattern exists */
			if (t == NULL)
			{
				if (j == 0) return(213);
				else return(217);
			}
			if (j == 0) Epat = t->i;
			else Pump[j].Epat = t->i;
			return(0);
		}
		else if (match(Tok[n - 2], w_EFFIC))             /* Pump efficiency */
		{
			if (j == 0)
			{
				if (!getfloat(Tok[n - 1], &y)) return(213);
				if (y <= 0.0) return(213);
				Epump = y;
			}
			else
			{
				t = findID(Tok[n - 1], Curvelist);         /* Check if curve exists */
				if (t == NULL) return(217);
				Pump[j].Ecurve = t->i;
			}
			return(0);
		}
		return(201);
	}


	int  EPANET::reportdata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes report options data
		**  Formats:
		**    PAGE     linesperpage
		**    STATUS   {NONE/YES/FULL}
		**    SUMMARY  {YES/NO}
		**    MESSAGES {YES/NO}
		**    ENERGY   {NO/YES}
		**    NODES    {NONE/ALL}
		**    NODES    node1  node2 ...
		**    LINKS    {NONE/ALL}
		**    LINKS    link1  link2 ...
		**    FILE     filename
		**    variable {YES/NO}
		**    variable {BELOW/ABOVE/PRECISION}  value
		**--------------------------------------------------------------
		*/
	{
		int    i, j, n;
		double  y;

		n = Ntokens - 1;
		if (n < 1) return(201);

		/* Value for page size */
		if (match(Tok[0], w_PAGE))
		{
			if (!getfloat(Tok[n], &y))   return(213);
			if (y < 0.0 || y > 255.0) return(213);
			PageSize = (int)y;
			return(0);
		}

		/* Request that status reports be written */
		if (match(Tok[0], w_STATUS))
		{
			if (match(Tok[n], w_NO))   Statflag = FALSE;
			if (match(Tok[n], w_YES))  Statflag = TRUE;
			if (match(Tok[n], w_FULL)) Statflag = FULL;
			return(0);
		}

		/* Request summary report */
		if (match(Tok[0], w_SUMMARY))
		{
			if (match(Tok[n], w_NO))  Summaryflag = FALSE;
			if (match(Tok[n], w_YES)) Summaryflag = TRUE;
			return(0);
		}

		/* Request error/warning message reporting */
		if (match(Tok[0], w_MESSAGES))
		{
			if (match(Tok[n], w_NO))  Messageflag = FALSE;
			if (match(Tok[n], w_YES)) Messageflag = TRUE;
			return(0);
		}


		/* Request an energy usage report */
		if (match(Tok[0], w_ENERGY))
		{
			if (match(Tok[n], w_NO))  Energyflag = FALSE;
			if (match(Tok[n], w_YES)) Energyflag = TRUE;
			return(0);
		}

		/* Particular reporting nodes specified */
		if (match(Tok[0], w_NODE))
		{
			if (match(Tok[n], w_NONE)) Nodeflag = 0;  /* No nodes */
			else if (match(Tok[n], w_ALL))  Nodeflag = 1;  /* All nodes */
			else
			{
				if (Nnodes == 0) return(208);
				for (i = 1; i <= n; i++)
				{
					if ((j = findnode(Tok[i])) == 0) return(208);
					Node[j].Rpt = 1;
				}
				Nodeflag = 2;
			}
			return(0);
		}

		/* Particular reporting links specified */
		if (match(Tok[0], w_LINK))
		{
			if (match(Tok[n], w_NONE)) Linkflag = 0;
			else if (match(Tok[n], w_ALL))  Linkflag = 1;
			else
			{
				if (Nlinks == 0) return(210);
				for (i = 1; i <= n; i++)
				{
					if ((j = findlink(Tok[i])) == 0) return(210);
					Link[j].Rpt = 1;
				}
				Linkflag = 2;
			}
			return(0);
		}

		/* Check if input is a reporting criterion. */

		/*** Special case needed to distinguish "HEAD" from "HEADLOSS" ***/            //(2.00.11 - LR)
		if (strcomp(Tok[0], w_HEADLOSS)) i = HEADLOSS;                              //(2.00.11 - LR)
		else i = findmatch(Tok[0], Fldname);                                         //(2.00.11 - LR)
		if (i >= 0)                                                                 //(2.00.11 - LR)
																					/*****************************************************************/            //(2.00.11 - LR)
		{
			if (i > FRICTION) return(201);
			if (Ntokens == 1 || match(Tok[1], w_YES))
			{
				Field[i].Enabled = TRUE;
				return(0);
			}
			if (match(Tok[1], w_NO))
			{
				Field[i].Enabled = FALSE;
				return(0);
			}
			if (Ntokens < 3) return(201);
			if (match(Tok[1], w_BELOW))  j = LOW;   /* Get relation operator */
			else if (match(Tok[1], w_ABOVE))  j = HI;    /* or precision keyword  */
			else if (match(Tok[1], w_PRECISION)) j = PREC;
			else return(201);
			if (!getfloat(Tok[2], &y)) return(201);
			if (j == PREC)
			{
				Field[i].Enabled = TRUE;
				Field[i].Precision = ROUND(y);
			}
			else Field[i].RptLim[j] = y;                /* Report limit value */
			return(0);
		}

		/* Name of external report file */
		if (match(Tok[0], w_FILE))
		{
			strncpy(Rpt2Fname, Tok[1], MAXFNAME);
			return(0);
		}

		/* If get to here then return error condition */
		return(201);
	}                        /* end of reportdata */


	int EPANET::timedata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes time options data
		**  Formats:
		**    STATISTIC                  {NONE/AVERAGE/MIN/MAX/RANGE}
		**    DURATION                   value   (units)
		**    HYDRAULIC TIMESTEP         value   (units)
		**    QUALITY TIMESTEP           value   (units)
		**    MINIMUM TRAVELTIME         value   (units)
		**    RULE TIMESTEP              value   (units)
		**    PATTERN TIMESTEP           value   (units)
		**    PATTERN START              value   (units)
		**    REPORT TIMESTEP            value   (units)
		**    REPORT START               value   (units)
		**    START CLOCKTIME            value   (AM PM)
		**-------------------------------------------------------------
		*/
	{
		int    n;
		long   t;
		double  y;

		n = Ntokens - 1;
		if (n < 1) return(201);

		/* Check if setting time statistic flag */
		if (match(Tok[0], w_STATISTIC))
		{
			if (match(Tok[n], w_NONE))  Tstatflag = SERIES;
			else if (match(Tok[n], w_NO))    Tstatflag = SERIES;
			else if (match(Tok[n], w_AVG))   Tstatflag = AVG;
			else if (match(Tok[n], w_MIN))   Tstatflag = MIN;
			else if (match(Tok[n], w_MAX))   Tstatflag = MAX;
			else if (match(Tok[n], w_RANGE)) Tstatflag = RANGE;
			else return(201);
			return(0);
		}

		/* Convert text time value to numerical value in seconds */
		/* Examples:
		**    5           = 5 * 3600 sec
		**    5 MINUTES   = 5 * 60   sec
		**    13:50       = 13*3600 + 50*60 sec
		**    1:50 pm     = (12+1)*3600 + 50*60 sec
		*/

		if (!getfloat(Tok[n], &y))
		{
			if ((y = hour(Tok[n], "")) < 0.0)
			{
				if ((y = hour(Tok[n - 1], Tok[n])) < 0.0) return(213);
			}
		}
		t = (long)(3600.0 * y);

		/* Process the value assigned to the matched parameter */
		if (match(Tok[0], w_DURATION))  Dur = t;      /* Simulation duration */
		else if (match(Tok[0], w_HYDRAULIC)) Hstep = t;    /* Hydraulic time Step */
		else if (match(Tok[0], w_QUALITY))   Qstep = t;    /* Quality time Step   */
		else if (match(Tok[0], w_RULE))      Rulestep = t; /* Rule time Step      */
		else if (match(Tok[0], w_MINIMUM))   return(0);    /* Not used anymore    */
		else if (match(Tok[0], w_PATTERN))
		{
			if (match(Tok[1], w_TIME))       Pstep = t;     /* Pattern time Step   */
			else if (match(Tok[1], w_START)) Pstart = t;    /* Pattern start time  */
			else return(201);
		}
		else if (match(Tok[0], w_REPORT))
		{
			if (match(Tok[1], w_TIME))  Rstep = t;     /* Reporting time Step  */
			else if (match(Tok[1], w_START)) Rstart = t;    /* Reporting start time */
			else return(201);
		}                                                 /* Simulation start time*/
		else if (match(Tok[0], w_START))    Tstart = t % SECperDAY;
		else return(201);
		return(0);
	}                        /* end of timedata */


	int  EPANET::optiondata()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: processes [OPTIONS] data
		**--------------------------------------------------------------
		*/
	{
		int i, n;

		n = Ntokens - 1;
		i = optionchoice(n);         /* Option is a named choice    */
		if (i >= 0) return(i);
		return(optionvalue(n));      /* Option is a numerical value */
	}                        /* end of optiondata */


	int  EPANET::optionchoice(int n)
		/*
		**--------------------------------------------------------------
		**  Input:   n = index of last input token saved in Tok[]
		**  Output:  returns error code or 0 if option belongs to
		**           those listed below, or -1 otherwise
		**  Purpose: processes fixed choice [OPTIONS] data
		**  Formats:
		**    UNITS               CFS/GPM/MGD/IMGD/AFD/LPS/LPM/MLD/CMH/CMD/SI
		**    PRESSURE            PSI/KPA/M
		**    HEADLOSS            H-W/D-W/C-M
		**    HYDRAULICS          USE/SAVE  filename
		**    QUALITY             NONE/AGE/TRACE/CHEMICAL  (TraceNode)
		**    MAP                 filename
		**    VERIFY              filename
		**    UNBALANCED          STOP/CONTINUE {Niter}
		**    PATTERN             id
		**--------------------------------------------------------------
		*/
	{
		/* Check if 1st token matches a parameter name and */
		/* process the input for the matched parameter     */
		if (n < 0) return(201);
		if (match(Tok[0], w_UNITS))
		{
			if (n < 1) return(0);
			else if (match(Tok[1], w_CFS))  Flowflag = CFS;
			else if (match(Tok[1], w_GPM))  Flowflag = GPM;
			else if (match(Tok[1], w_AFD))  Flowflag = AFD;
			else if (match(Tok[1], w_MGD))  Flowflag = MGD;
			else if (match(Tok[1], w_IMGD)) Flowflag = IMGD;
			else if (match(Tok[1], w_LPS))  Flowflag = LPS;
			else if (match(Tok[1], w_LPM))  Flowflag = LPM;
			else if (match(Tok[1], w_CMH))  Flowflag = CMH;
			else if (match(Tok[1], w_CMD))  Flowflag = CMD;
			else if (match(Tok[1], w_MLD))  Flowflag = MLD;
			else if (match(Tok[1], w_SI))   Flowflag = LPS;
			else return(201);
		}
		else if (match(Tok[0], w_PRESSURE))
		{
			if (n < 1) return(0);
			else if (match(Tok[1], w_PSI))    Pressflag = PSI;
			else if (match(Tok[1], w_KPA))    Pressflag = KPA;
			else if (match(Tok[1], w_METERS)) Pressflag = METERS;
			else return(201);
		}
		else if (match(Tok[0], w_HEADLOSS))
		{
			if (n < 1) return(0);
			else if (match(Tok[1], w_HW)) Formflag = HW;
			else if (match(Tok[1], w_DW)) Formflag = DW;
			else if (match(Tok[1], w_CM)) Formflag = CM;
			else return(201);
		}
		else if (match(Tok[0], w_HYDRAULIC))
		{
			if (n < 2) return(0);
			else if (match(Tok[1], w_USE))  Hydflag = USE;
			else if (match(Tok[1], w_SAVE)) Hydflag = SAVE;
			else return(201);
			strncpy(HydFname, Tok[2], MAXFNAME);
		}
		else if (match(Tok[0], w_QUALITY))
		{
			if (n < 1) return(0);
			else if (match(Tok[1], w_NONE))  Qualflag = NONE;
			else if (match(Tok[1], w_CHEM))  Qualflag = CHEM;
			else if (match(Tok[1], w_AGE))   Qualflag = AGE;
			else if (match(Tok[1], w_TRACE)) Qualflag = TRACE;
			else
			{
				Qualflag = CHEM;
				strncpy(ChemName, Tok[1], MAXID);
				if (n >= 2) strncpy(ChemUnits, Tok[2], MAXID);
			}
			if (Qualflag == TRACE)                  /* Source tracing option */
			{
				/* Copy Trace Node ID to Tok[0] for error reporting */
				strcpy(Tok[0], "");
				if (n < 2) return(212);
				strcpy(Tok[0], Tok[2]);
				TraceNode = findnode(Tok[2]);
				if (TraceNode == 0) return(212);
				strncpy(ChemName, u_PERCENT, MAXID);
				strncpy(ChemUnits, Tok[2], MAXID);
			}
			if (Qualflag == AGE)
			{
				strncpy(ChemName, w_AGE, MAXID);
				strncpy(ChemUnits, u_HOURS, MAXID);
			}
		}
		else if (match(Tok[0], w_MAP))
		{
			if (n < 1) return(0);
			strncpy(MapFname, Tok[1], MAXFNAME);        /* Map file name */
		}
		else if (match(Tok[0], w_VERIFY))
		{
			/* Backward compatibility for verification file */
		}
		else if (match(Tok[0], w_UNBALANCED))         /* Unbalanced option */
		{
			if (n < 1) return(0);
			if (match(Tok[1], w_STOP)) ExtraIter = -1;
			else if (match(Tok[1], w_CONTINUE))
			{
				if (n >= 2) ExtraIter = atoi(Tok[2]);
				else ExtraIter = 0;
			}
			else return(201);
		}
		else if (match(Tok[0], w_PATTERN))            /* Pattern option */
		{
			if (n < 1) return(0);
			strncpy(DefPatID, Tok[1], MAXID);
		}
		else return(-1);
		return(0);
	}                        /* end of optionchoice */


	int  EPANET::optionvalue(int n)
		/*
		**-------------------------------------------------------------
		**  Input:   *line = line read from input file
		**  Output:  returns error code
		**  Purpose: processes numerical value [OPTIONS] data
		**  Formats:
		**    DEMAND MULTIPLIER   value
		**    EMITTER EXPONENT    value
		**    VISCOSITY           value
		**    DIFFUSIVITY         value
		**    SPECIFIC GRAVITY    value
		**    TRIALS              value
		**    ACCURACY            value
		**    TOLERANCE           value
		**    SEGMENTS            value  (not used)
		**  ------ Undocumented Options -----
		**    HTOL                value
		**    QTOL                value
		**    RQTOL               value
		**    CHECKFREQ           value
		**    MAXCHECK            value
		**    DAMPLIMIT           value                                                //(2.00.12 - LR)
		**--------------------------------------------------------------
		*/
	{
		int    nvalue = 1;   /* Index of token with numerical value */
		double  y;

		/* Check for obsolete SEGMENTS keyword */
		if (match(Tok[0], w_SEGMENTS)) return(0);

		/* Check for missing value (which is permissible) */
		if (match(Tok[0], w_SPECGRAV) || match(Tok[0], w_EMITTER)
			|| match(Tok[0], w_DEMAND)) nvalue = 2;
		if (n < nvalue) return(0);

		/* Check for valid numerical input */
		if (!getfloat(Tok[nvalue], &y)) return(213);

		/* Check for WQ tolerance option (which can be 0) */
		if (match(Tok[0], w_TOLERANCE))
		{
			if (y < 0.0) return(213);
			Ctol = y;         /* Quality tolerance*/
			return(0);
		}

		/* Check for Diffusivity option */
		if (match(Tok[0], w_DIFFUSIVITY))
		{
			if (y < 0.0) return(213);
			Diffus = y;
			return(0);
		}

		/* Check for Damping Limit option */                                           //(2.00.12 - LR)
		if (match(Tok[0], w_DAMPLIMIT))
		{
			DampLimit = y;
			return(0);
		}

		/* All other options must be > 0 */
		if (y <= 0.0) return(213);

		/* Assign value to specified option */
		if (match(Tok[0], w_VISCOSITY))   Viscos = y;       /* Viscosity */
		else if (match(Tok[0], w_SPECGRAV))    SpGrav = y;       /* Spec. gravity */
		else if (match(Tok[0], w_TRIALS))      MaxIter = (int)y; /* Max. trials */
		else if (match(Tok[0], w_ACCURACY))                      /* Accuracy */
		{
			y = MAX(y, 1.e-5);
			y = MIN(y, 1.e-1);
			Hacc = y;
		}
		else if (match(Tok[0], w_HTOL))        Htol = y;
		else if (match(Tok[0], w_QTOL))        Qtol = y;
		else if (match(Tok[0], w_RQTOL))
		{
			if (y >= 1.0) return(213);
			RQtol = y;
		}
		else if (match(Tok[0], w_CHECKFREQ))   CheckFreq = (int)y;
		else if (match(Tok[0], w_MAXCHECK))    MaxCheck = (int)y;
		else if (match(Tok[0], w_EMITTER))     Qexp = 1.0 / y;
		else if (match(Tok[0], w_DEMAND))      Dmult = y;
		else return(201);
		return(0);
	}                        /* end of optionvalue */


	int  EPANET::getpumpcurve(int n)
		/*
		**--------------------------------------------------------
		**  Input:   n = number of parameters for pump curve
		**  Output:  returns error code
		**  Purpose: processes pump curve data for Version 1.1-
		**           style input data
		**  Notes:
		**    1. Called by pumpdata() in INPUT3.C
		**    2. Current link index & pump index of pump being
		**       processed is found in global variables Nlinks
		**       and Npumps, respectively
		**    3. Curve data read from input line is found in
		**       global variables X[0],...X[n-1]
		**---------------------------------------------------------
		*/
	{
		double a, b, c, h0, h1, h2, q1, q2;

		if (n == 1)                /* Const. HP curve       */
		{
			if (X[0] <= 0.0) return(202);
			Pump[Npumps].Ptype = CONST_HP;
			Link[Nlinks].Km = X[0];
		}
		else
		{
			if (n == 2)             /* Generic power curve   */
			{
				q1 = X[1];
				h1 = X[0];
				h0 = 1.33334 * h1;
				q2 = 2.0 * q1;
				h2 = 0.0;
			}
			else if (n >= 5)        /* 3-pt. power curve     */
			{
				h0 = X[0];
				h1 = X[1];
				q1 = X[2];
				h2 = X[3];
				q2 = X[4];
			}
			else return(202);
			Pump[Npumps].Ptype = POWER_FUNC;
			if (!powercurve(h0, h1, h2, q1, q2, &a, &b, &c)) return(206);
			Pump[Npumps].H0 = -a;
			Pump[Npumps].R = -b;
			Pump[Npumps].N = c;
			Pump[Npumps].Q0 = q1;
			Pump[Npumps].Qmax = pow((-a / b), (1.0 / c));
			Pump[Npumps].Hmax = h0;
		}
		return(0);
	}


	int  EPANET::powercurve(double h0, double h1, double h2, double q1,
		double q2, double *a, double *b, double *c)
		/*
		**---------------------------------------------------------
		**  Input:   h0 = shutoff head
		**           h1 = design head
		**           h2 = head at max. flow
		**           q1 = design flow
		**           q2 = max. flow
		**  Output:  *a, *b, *c = pump curve coeffs. (H = a-bQ^c),
		**           Returns 1 if sucessful, 0 otherwise.
		**  Purpose: computes coeffs. for pump curve
		**----------------------------------------------------------
		*/
	{
		double h4, h5;
		if (
			h0 < TINY ||
			h0 - h1 < TINY ||
			h1 - h2 < TINY ||
			q1 < TINY ||
			q2 - q1 < TINY
			) return(0);
		*a = h0;
		h4 = h0 - h1;
		h5 = h0 - h2;
		*c = log(h5 / h4) / log(q2 / q1);
		if (*c <= 0.0 || *c > 20.0) return(0);
		*b = -h4 / pow(q1, *c);

		/*** Updated 6/24/02 ***/
		if (*b >= 0.0) return(0);

		return(1);
	}


	int  EPANET::valvecheck(int type, int j1, int j2)
		/*
		**--------------------------------------------------------------
		**  Input:   type = valve type
		**           j1   = index of upstream node
		**           j2   = index of downstream node
		**  Output:  returns 1 for legal connection, 0 otherwise
		**  Purpose: checks for legal connections between PRVs & PSVs
		**--------------------------------------------------------------
		*/
	{
		int  k, vk, vj1, vj2, vtype;

		/* Examine each existing valve */
		for (k = 1; k <= Nvalves; k++)
		{
			vk = Valve[k].Link;
			vj1 = Link[vk].N1;
			vj2 = Link[vk].N2;
			vtype = Link[vk].Type;

			/* Cannot have two PRVs sharing downstream nodes or in series */
			if (vtype == PRV && type == PRV)
			{
				if (vj2 == j2 ||
					vj2 == j1 ||
					vj1 == j2) return(0);
			}

			/* Cannot have two PSVs sharing upstream nodes or in series */
			if (vtype == PSV && type == PSV)
			{
				if (vj1 == j1 ||
					vj1 == j2 ||
					vj2 == j1) return(0);
			}

			/* Cannot have PSV connected to downstream node of PRV */
			if (vtype == PSV && type == PRV && vj1 == j2) return(0);
			if (vtype == PRV && type == PSV && vj2 == j1) return(0);

			/*** Updated 3/1/01 ***/
			/* Cannot have PSV connected to downstream node of FCV */
			/* nor have PRV connected to upstream node of FCV */
			if (vtype == FCV && type == PSV && vj2 == j1) return(0);
			if (vtype == FCV && type == PRV && vj1 == j2) return(0);

			/*** Updated 4/14/05 ***/
			if (vtype == PSV && type == FCV && vj1 == j2) return (0);
			if (vtype == PRV && type == FCV && vj2 == j1) return (0);
		}
		return(1);
	}                   /* End of valvecheck */


	void  EPANET::changestatus(int j, char status, double y)
		/*
		**--------------------------------------------------------------
		**  Input:   j      = link index
		**           status = status setting (OPEN, CLOSED)
		**           y      = numerical setting (pump speed, valve
		**                    setting)
		**  Output:  none
		**  Purpose: changes status or setting of a link
		**
		**  NOTE: If status = ACTIVE, then a numerical setting (y) was
		**        supplied. If status = OPEN/CLOSED, then numerical
		**        setting is 0.
		**--------------------------------------------------------------
		*/
	{
		if (Link[j].Type == PIPE || Link[j].Type == GPV)
		{
			if (status != ACTIVE) Link[j].Stat = status;
		}
		else if (Link[j].Type == PUMP)
		{
			if (status == ACTIVE)
			{
				Link[j].Kc = y;
				status = OPEN;
				if (y == 0.0) status = CLOSED;
			}
			else if (status == OPEN) Link[j].Kc = 1.0;
			Link[j].Stat = status;
		}
		else if (Link[j].Type >= PRV)
		{
			Link[j].Kc = y;
			Link[j].Stat = status;
			if (status != ACTIVE) Link[j].Kc = MISSING;
		}
	}                        /* end of changestatus */

	/*rules*/

	void EPANET::initrules()
		/*
		**--------------------------------------------------------------
		**    Initializes rule base.
		**    Called by ENopen() in EPANET.C module
		**--------------------------------------------------------------
		*/
	{
		RuleState = r_PRIORITY;
		Rule = NULL;
	}


	void EPANET::addrule(char *tok)
		/*
		**--------------------------------------------------------------
		**    Updates rule count if RULE keyword found in line of input.
		**    Called by netsize() in INPUT2.C module.
		**--------------------------------------------------------------
		*/
	{
		if (match(tok, w_RULE)) MaxRules++;
	}


	int  EPANET::allocrules()
		/*
		**--------------------------------------------------------------
		**    Allocates memory for rule-based controls.
		**    Called by allocdata() in EPANET.C module.
		**--------------------------------------------------------------
		*/
	{
		Rule = (aRule *)calloc(MaxRules + 1, sizeof(aRule));
		if (Rule == NULL) return(101);
		else return(0);
	}


	void EPANET::freerules()
		/*
		**--------------------------------------------------------------
		**    Frees memory used for rule-based controls.
		**    Called by freedata() in EPANET.C module.
		**--------------------------------------------------------------
		*/
	{
		clearrules();
		free(Rule);
	}


	int EPANET::checkrules(long dt)
		/*
		**-----------------------------------------------------
		**    Checks which rules should fire at current time.
		**    Called by ruletimestep() in HYDRAUL.C.
		**-----------------------------------------------------
		*/
	{
		int i,
			r;    /* Number of actions actually taken */

				  /* Start of rule evaluation time interval */
		Time1 = Htime - dt + 1;

		/* Iterate through each rule */
		ActList = NULL;
		r = 0;
		for (i = 1; i <= Nrules; i++)
		{
			/* If premises true, add THEN clauses to action list. */
			if (evalpremises(i) == TRUE) updateactlist(i, Rule[i].Tchain);

			/* If premises false, add ELSE actions to list. */
			else
			{
				if (Rule[i].Fchain != NULL) updateactlist(i, Rule[i].Fchain);
			}
		}

		/* Execute actions then clear list. */
		if (ActList != NULL) r = takeactions();
		clearactlist();
		return(r);
	}


	int  EPANET::ruledata()
		/*
		**--------------------------------------------------------------
		**    Parses a line from [RULES] section of input.
		**    Called by newline() in INPUT2.C module.
		**    Tok[] is global array of tokens parsed from input line.
		**--------------------------------------------------------------
		*/
	{
		int    key,                      /* Keyword code */
			err;

		/* Exit if current rule has an error */
		if (RuleState == r_ERROR) return(0);

		/* Find the key word that begins the rule statement */
		err = 0;
		key = findmatch(Tok[0], Ruleword);
		switch (key)
		{
		case -1:     err = 201;      /* Unrecognized keyword */
			break;
		case r_RULE: Nrules++;
			newrule();
			RuleState = r_RULE;
			break;
		case r_IF:   if (RuleState != r_RULE)
		{
			err = 221;   /* Mis-placed IF clause */
			break;
		}
				 RuleState = r_IF;
				 err = newpremise(r_AND);
				 break;
		case r_AND:  if (RuleState == r_IF) err = newpremise(r_AND);
				  else if (RuleState == r_THEN || RuleState == r_ELSE)
			err = newaction();
				  else err = 221;
			break;
		case r_OR:   if (RuleState == r_IF) err = newpremise(r_OR);
				 else err = 221;
			break;
		case r_THEN: if (RuleState != r_IF)
		{
			err = 221;   /* Mis-placed THEN clause */
			break;
		}
				   RuleState = r_THEN;
				   err = newaction();
				   break;
		case r_ELSE: if (RuleState != r_THEN)
		{
			err = 221;   /* Mis-placed ELSE clause */
			break;
		}
				   RuleState = r_ELSE;
				   err = newaction();
				   break;
		case r_PRIORITY: if (RuleState != r_THEN && RuleState != r_ELSE)
		{
			err = 221;
			break;
		}
					   RuleState = r_PRIORITY;
					   err = newpriority();
					   break;
		default:         err = 201;
		}

		/* Set RuleState to r_ERROR if errors found */
		if (err)
		{
			RuleState = r_ERROR;
			ruleerrmsg(err);
			err = 200;
		}
		return(err);
	}


	void  EPANET::clearactlist()
		/*
		**----------------------------------------------------------
		**    Clears memory used for action list
		**----------------------------------------------------------
		*/
	{
		ActItem *a;
		ActItem *anext;
		a = ActList;
		while (a != NULL)
		{
			anext = a->next;
			free(a);
			a = anext;
		}
	}


	void  EPANET::clearrules()
		/*
		**-----------------------------------------------------------
		**    Clears memory used for premises & actions for all rules
		**-----------------------------------------------------------
		*/
	{
		Premise *p;
		Premise *pnext;
		Action *a;
		Action *anext;
		int i;
		for (i = 1; i <= Nrules; i++)
		{
			p = Rule[i].Pchain;
			while (p != NULL)
			{
				pnext = p->next;
				free(p);
				p = pnext;
			}
			a = Rule[i].Tchain;
			while (a != NULL)
			{
				anext = a->next;
				free(a);
				a = anext;
			}
			a = Rule[i].Fchain;
			while (a != NULL)
			{
				anext = a->next;
				free(a);
				a = anext;
			}
		}
	}


	void  EPANET::newrule()
		/*
		**----------------------------------------------------------
		**    Adds new rule to rule base
		**----------------------------------------------------------
		*/
	{
		strncpy(Rule[Nrules].label, Tok[1], MAXID);
		Rule[Nrules].Pchain = NULL;
		Rule[Nrules].Tchain = NULL;
		Rule[Nrules].Fchain = NULL;
		Rule[Nrules].priority = 0.0;
		Plast = NULL;
	}


	int  EPANET::newpremise(int logop)
		/*
		**--------------------------------------------------------------------
		**   Adds new premise to current rule.
		**   Formats are:
		**     IF/AND/OR <object> <id> <variable> <operator> <value>
		**     IF/AND/OR  SYSTEM <variable> <operator> <value> (units)
		**
		**   Calls findmatch() and hour() in INPUT2.C.
		**   Calls findnode() and findlink() in EPANET.C.
		**---------------------------------------------------------------------
		*/
	{
		int   i, j, k, m, r, s, v;
		double x;
		Premise *p;

		/* Check for correct number of tokens */
		if (Ntokens != 5 && Ntokens != 6) return(201);

		/* Find network object & id if present */
		i = findmatch(Tok[1], Object);
		if (i == r_SYSTEM)
		{
			j = 0;
			v = findmatch(Tok[2], Varword);
			if (v != r_DEMAND && v != r_TIME && v != r_CLOCKTIME) return(201);
		}
		else
		{
			v = findmatch(Tok[3], Varword);
			if (v < 0) return(201);
			switch (i)
			{
			case r_NODE:
			case r_JUNC:
			case r_RESERV:
			case r_TANK:   k = r_NODE; break;
			case r_LINK:
			case r_PIPE:
			case r_PUMP:
			case r_VALVE:  k = r_LINK; break;
			default: return(201);
			}
			i = k;
			if (i == r_NODE)
			{
				j = findnode(Tok[2]);
				if (j == 0) return(203);
				switch (v)
				{
				case r_DEMAND:
				case r_HEAD:
				case r_GRADE:
				case r_LEVEL:
				case r_PRESSURE: break;

					/*** Updated 9/7/00 ***/
				case r_FILLTIME:
				case r_DRAINTIME: if (j <= Njuncs) return(201); break;

				default: return(201);
				}
			}
			else
			{
				j = findlink(Tok[2]);
				if (j == 0) return(204);
				switch (v)
				{
				case r_FLOW:
				case r_STATUS:
				case r_SETTING: break;
				default: return(201);
				}
			}
		}

		/* Parse relational operator (r) and check for synonyms */
		if (i == r_SYSTEM) m = 3;
		else m = 4;
		k = findmatch(Tok[m], Operator);
		if (k < 0) return(201);
		switch (k)
		{
		case IS:    r = EQ; break;
		case NOT:   r = NE; break;
		case BELOW: r = LT; break;
		case ABOVE: r = GT; break;
		default:    r = k;
		}

		/* Parse for status (s) or numerical value (x) */
		s = 0;
		x = MISSING;
		if (v == r_TIME || v == r_CLOCKTIME)
		{
			if (Ntokens == 6)
				x = hour(Tok[4], Tok[5]) * 3600.;
			else
				x = hour(Tok[4], "") * 3600.;
			if (x < 0.0) return(202);
		}
		else if ((k = findmatch(Tok[Ntokens - 1], Value)) > IS_NUMBER) s = k;
		else
		{
			if (!getfloat(Tok[Ntokens - 1], &x)) return(202);
			if (v == r_FILLTIME || v == r_DRAINTIME) x = x * 3600.0;                   //(2.00.11 - LR)
		}



		/* Create new premise structure */
		p = (Premise *)malloc(sizeof(Premise));
		if (p == NULL) return(101);
		p->object = i;
		p->index = j;
		p->variable = v;
		p->relop = r;
		p->logop = logop;
		p->status = s;
		p->value = x;

		/* Add premise to current rule's premise list */
		p->next = NULL;
		if (Plast == NULL) Rule[Nrules].Pchain = p;
		else Plast->next = p;
		Plast = p;
		return(0);
	}


	int  EPANET::newaction()
		/*
		**----------------------------------------------------------
		**   Adds new action to current rule.
		**   Format is:
		**      THEN/ELSE/AND LINK <id> <variable> IS <value>
		**
		**   Calls findlink() from EPANET.C.
		**   Calls getfloat() and findmatch() from INPUT2.C.
		**----------------------------------------------------------
		*/
	{
		int   j, k, s;
		double x;
		Action *a;

		/* Check for correct number of tokens */
		if (Ntokens != 6) return(201);

		/* Check that link exists */
		j = findlink(Tok[2]);
		if (j == 0) return(204);

		/***  Updated 9/7/00  ***/
		/* Cannot control a CV */
		if (Link[j].Type == CV) return(207);

		/* Find value for status or setting */
		s = -1;
		x = MISSING;
		if ((k = findmatch(Tok[5], Value)) > IS_NUMBER) s = k;
		else
		{
			if (!getfloat(Tok[5], &x)) return(202);
			if (x < 0.0) return(202);
		}

		/*** Updated 9/7/00 ***/
		/* Cannot change setting for a GPV ***/
		if (x != MISSING && Link[j].Type == GPV) return(202);

		/*** Updated 3/1/01 ***/
		/* Set status for pipe in case setting was specified */
		if (x != MISSING && Link[j].Type == PIPE)
		{
			if (x == 0.0) s = IS_CLOSED;
			else          s = IS_OPEN;
			x = MISSING;
		}

		/* Create a new action structure */
		a = (Action *)malloc(sizeof(Action));
		if (a == NULL) return(101);
		a->link = j;
		a->status = s;
		a->setting = x;

		/* Add action to current rule's action list */
		if (RuleState == r_THEN)
		{
			a->next = Rule[Nrules].Tchain;
			Rule[Nrules].Tchain = a;
		}
		else
		{
			a->next = Rule[Nrules].Fchain;
			Rule[Nrules].Fchain = a;
		}
		return(0);
	}


	int  EPANET::newpriority()
		/*
		**---------------------------------------------------
		**    Adds priority rating to current rule
		**---------------------------------------------------
		*/
	{
		double x;
		if (!getfloat(Tok[1], &x)) return(202);
		Rule[Nrules].priority = x;
		return(0);
	}


	int  EPANET::evalpremises(int i)
		/*
		**----------------------------------------------------------
		**    Checks if premises to rule i are true
		**----------------------------------------------------------
		*/
	{
		int result;
		Premise *p;

		result = TRUE;
		p = Rule[i].Pchain;
		while (p != NULL)
		{
			if (p->logop == r_OR)
			{
				if (result == FALSE)
				{
					result = checkpremise(p);
				}
			}
			else
			{
				if (result == FALSE) return(FALSE);
				result = checkpremise(p);
			}
			p = p->next;
		}
		return(result);
	}


	int  EPANET::checkpremise(Premise *p)
		/*
		**----------------------------------------------------------
		**    Checks if a particular premise is true
		**----------------------------------------------------------
		*/
	{
		if (p->variable == r_TIME || p->variable == r_CLOCKTIME)
			return(checktime(p));
		else if (p->status > IS_NUMBER)
			return(checkstatus(p));
		else
			return(checkvalue(p));
	}


	int  EPANET::checktime(Premise *p)
		/*
		**------------------------------------------------------------
		**    Checks if condition on system time holds
		**------------------------------------------------------------
		*/
	{
		char  flag;
		long  t1, t2, x;

		/* Get start and end of rule evaluation time interval */
		if (p->variable == r_TIME)
		{
			t1 = Time1;
			t2 = Htime;
		}
		else if (p->variable == r_CLOCKTIME)
		{
			t1 = (Time1 + Tstart) % SECperDAY;
			t2 = (Htime + Tstart) % SECperDAY;
		}
		else return(0);

		/* Test premise's time */
		x = (long)(p->value);
		switch (p->relop)
		{
			/* For inequality, test against current time */
		case LT: if (t2 >= x) return(0); break;
		case LE: if (t2 > x) return(0); break;
		case GT: if (t2 <= x) return(0); break;
		case GE: if (t2 < x) return(0); break;

			/* For equality, test if within interval */
		case EQ:
		case NE:
			flag = FALSE;
			if (t2 < t1)     /* E.g., 11:00 am to 1:00 am */
			{
				if (x >= t1 || x <= t2) flag = TRUE;
			}
			else
			{
				if (x >= t1 && x <= t2) flag = TRUE;
			}
			if (p->relop == EQ && flag == FALSE) return(0);
			if (p->relop == NE && flag == TRUE)  return(0);
			break;
		}

		/* If we get to here then premise was satisfied */
		return(1);
	}


	int  EPANET::checkstatus(Premise *p)
		/*
		**------------------------------------------------------------
		**    Checks if condition on link status holds
		**------------------------------------------------------------
		*/
	{
		char i;
		int  j;
		switch (p->status)
		{
		case IS_OPEN:
		case IS_CLOSED:
		case IS_ACTIVE:
			i = S[p->index];
			if (i <= CLOSED) j = IS_CLOSED;
			else if (i == ACTIVE) j = IS_ACTIVE;
			else                  j = IS_OPEN;
			if (j == p->status &&
				p->relop == EQ)     return(1);
			if (j != p->status &&
				p->relop == NE) return(1);
		}
		return(0);
	}


	int  EPANET::checkvalue(Premise *p)
		/*
		**----------------------------------------------------------
		**    Checks if numerical condition on a variable is true.
		**    Uses tolerance of 0.001 when testing conditions.
		**----------------------------------------------------------
		*/
	{
		int   i, j, v;
		double x,
			tol = 1.e-3;

		i = p->index;
		v = p->variable;
		switch (v)
		{

			/*** Updated 10/25/00 ***/
		case r_DEMAND:    if (p->object == r_SYSTEM) x = Dsystem * Ucf[DEMAND];
					 else x = D[i] * Ucf[DEMAND];
			break;

		case r_HEAD:
		case r_GRADE:     x = H[i] * Ucf[HEAD];
			break;
		case r_PRESSURE:  x = (H[i] - Node[i].El) * Ucf[PRESSURE];
			break;
		case r_LEVEL:     x = (H[i] - Node[i].El) * Ucf[HEAD];
			break;
		case r_FLOW:      x = ABS(Q[i]) * Ucf[FLOW];
			break;
		case r_SETTING:   if (K[i] == MISSING) return(0);
			x = K[i];
			switch (Link[i].Type)
			{
			case PRV:
			case PSV:
			case PBV:  x = x * Ucf[PRESSURE]; break;
			case FCV:  x = x * Ucf[FLOW];     break;
			}
			break;
		case r_FILLTIME:  if (i <= Njuncs) return(0);
			j = i - Njuncs;
			if (Tank[j].A == 0.0) return(0);
			if (D[i] <= TINY) return(0);
			x = (Tank[j].Vmax - Tank[j].V) / D[i];
			break;
		case r_DRAINTIME: if (i <= Njuncs) return(0);
			j = i - Njuncs;
			if (Tank[j].A == 0.0) return(0);
			if (D[i] >= -TINY) return(0);
			x = (Tank[j].Vmin - Tank[j].V) / D[i];
			break;
		default:          return(0);
		}
		switch (p->relop)
		{
		case EQ:        if (ABS(x - p->value) > tol) return(0);
			break;
		case NE:        if (ABS(x - p->value) < tol) return(0);
			break;
		case LT:        if (x > p->value + tol) return(0); break;
		case LE:        if (x > p->value - tol) return(0); break;
		case GT:        if (x < p->value - tol) return(0); break;
		case GE:        if (x < p->value + tol) return(0); break;
		}
		return(1);
	}


	void  EPANET::updateactlist(int i, Action *actions)
		/*
		**---------------------------------------------------
		**    Adds rule's actions to action list
		**---------------------------------------------------
		*/
	{
		ActItem *item;
		Action *a;

		/* Iterate through each action of Rule i */
		a = actions;
		while (a != NULL)
		{
			/* Add action to list if not already on it */
			if (!checkaction(i, a))
			{
				item = (ActItem *)malloc(sizeof(ActItem));
				if (item != NULL)
				{
					item->action = a;
					item->ruleindex = i;
					item->next = ActList;
					ActList = item;
				}
			}
			a = a->next;
		}
	}


	int  EPANET::checkaction(int i, Action *a)
		/*
		**-----------------------------------------------------------
		**    Checks if an action is already on the Action List
		**-----------------------------------------------------------
		*/
	{
		int i1, k, k1;
		ActItem *item;
		Action *a1;

		/* Search action list for link named in action */
		k = a->link;                 /* Action applies to link k */
		item = ActList;
		while (item != NULL)
		{
			a1 = item->action;
			i1 = item->ruleindex;
			k1 = a1->link;

			/* If link on list then replace action if rule has higher priority. */
			if (k1 == k)
			{
				if (Rule[i].priority > Rule[i1].priority)
				{
					item->action = a;
					item->ruleindex = i;
				}
				return(1);
			}
			item = item->next;
		}
		return(0);
	}


	int  EPANET::takeactions()
		/*
		**-----------------------------------------------------------
		**    Implements actions on action list
		**-----------------------------------------------------------
		*/
	{
		Action *a;
		ActItem *item;
		char   flag;
		int    k, s, n;
		double  tol = 1.e-3,
			v, x;

		n = 0;
		item = ActList;
		while (item != NULL)
		{
			flag = FALSE;
			a = item->action;
			k = a->link;
			s = S[k];
			v = K[k];
			x = a->setting;

			/* Switch link from closed to open */
			if (a->status == IS_OPEN && s <= CLOSED)
			{
				setlinkstatus(k, 1, &S[k], &K[k]);
				flag = TRUE;
			}

			/* Switch link from not closed to closed */
			else if (a->status == IS_CLOSED && s > CLOSED)
			{
				setlinkstatus(k, 0, &S[k], &K[k]);
				flag = TRUE;
			}

			/* Change link's setting */
			else if (x != MISSING)
			{
				switch (Link[k].Type)
				{
				case PRV:
				case PSV:
				case PBV:    x = x / Ucf[PRESSURE];  break;
				case FCV:    x = x / Ucf[FLOW];      break;
				}
				if (ABS(x - v) > tol)
				{
					setlinksetting(k, x, &S[k], &K[k]);
					flag = TRUE;
				}
			}

			/* Report rule action */
			if (flag == TRUE)
			{
				n++;
				if (Statflag) writeruleaction(k, Rule[item->ruleindex].label);
			}

			/* Move to next action on list */
			item = item->next;
		}
		return(n);
	}


	void  EPANET::ruleerrmsg(int err)
		/*
		**-----------------------------------------------------------
		**    Reports error message
		**-----------------------------------------------------------
		*/
	{
		int    i;
		char   label[81];
		char   fmt[256];
		switch (err)
		{
		case 201:   strcpy(fmt, R_ERR201);  break;
		case 202:   strcpy(fmt, R_ERR202);  break;
		case 203:   strcpy(fmt, R_ERR203);  break;
		case 204:   strcpy(fmt, R_ERR204);  break;

			/***  Updated on 9/7/00  ***/
		case 207:   strcpy(fmt, R_ERR207);  break;

		case 221:   strcpy(fmt, R_ERR221);  break;
		default:    return;
		}
		if (Nrules > 0)
		{
			strcpy(label, t_RULE);
			strcat(label, " ");
			strcat(label, Rule[Nrules].label);
		}
		else strcpy(label, t_RULES_SECT);
		sprintf(Msg, fmt);
		strcat(Msg, label);
		strcat(Msg, ":");
		writeline(Msg);
		strcpy(fmt, Tok[0]);
		for (i = 1; i < Ntokens; i++)
		{
			strcat(fmt, " ");
			strcat(fmt, Tok[i]);
		}
		writeline(fmt);
	}

	/*report*/
	int EPANET::writereport()
		/*
		**------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: writes formatted output report to file
		**
		**   Calls strcomp() from the EPANET.C module.
		**------------------------------------------------------
		*/
	{
		char  tflag;
		FILE *tfile;
		int   errcode = 0;

		/* If no secondary report file specified then    */
		/* write formatted output to primary report file. */
		Fprinterr = FALSE;
		if (Rptflag && strlen(Rpt2Fname) == 0 && RptFile != NULL)
		{
			writecon(FMT17);
			writecon(Rpt1Fname);
			if (Energyflag) writeenergy();
			errcode = writeresults();
		}

		/* A secondary report file was specified */
		else if (strlen(Rpt2Fname) > 0)
		{

			/* If secondary report file has same name as either input */
			/* or primary report file then use primary report file.   */
			if (strcomp(Rpt2Fname, InpFname) ||
				strcomp(Rpt2Fname, Rpt1Fname))
			{
				writecon(FMT17);
				writecon(Rpt1Fname);
				if (Energyflag) writeenergy();
				errcode = writeresults();
			}

			/* Otherwise write report to secondary report file. */
			else
			{

				/* Try to open file */
				tfile = RptFile;
				tflag = Rptflag;
				if ((RptFile = fopen(Rpt2Fname, "wt")) == NULL)
				{
					RptFile = tfile;
					Rptflag = tflag;
					errcode = 303;
				}

				/* Write full formatted report to file */
				else
				{
					Rptflag = 1;
					writecon(FMT17);
					writecon(Rpt2Fname);
					writelogo();
					if (Summaryflag) writesummary();
					if (Energyflag)  writeenergy();
					errcode = writeresults();
					fclose(RptFile);
					RptFile = tfile;
					Rptflag = tflag;
				}
			}
		}

		/* Special error handler for write-to-file error */
		if (Fprinterr) errmsg(309);
		return(errcode);
	}                        /* End of writereport */


	void EPANET::writelogo()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: writes program logo to report file.
		**--------------------------------------------------------------
		*/
	{
		int    i;
		time_t timer;         /* time_t structure & functions time() & */
							  /* ctime() are defined in time.h         */
		time(&timer);
		strcpy(DateStamp, ctime(&timer));
		PageNum = 1;
		LineNum = 2;
		fprintf(RptFile, FMT18);
		fprintf(RptFile, "%s", DateStamp);
		for (i = 0; LogoTxt[i] != NULL; i++) writeline(LogoTxt[i]);
		writeline("");
	}                        /* End of writelogo */


	void EPANET::writesummary()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: writes summary system information to report file
		**--------------------------------------------------------------
		*/
	{
		char s[MAXFNAME + 1];
		int  i;
		int  nres = 0;

		for (i = 0; i < 3; i++)
		{
			if (strlen(Title[i]) > 0)
			{
				sprintf(s, "%-.70s", Title[i]);
				writeline(s);
			}
		}
		writeline(" ");
		sprintf(s, FMT19, InpFname);
		writeline(s);
		sprintf(s, FMT20, Njuncs);
		writeline(s);
		for (i = 1; i <= Ntanks; i++)
			if (Tank[i].A == 0.0) nres++;
		sprintf(s, FMT21a, nres);
		writeline(s);
		sprintf(s, FMT21b, Ntanks - nres);
		writeline(s);
		sprintf(s, FMT22, Npipes);
		writeline(s);
		sprintf(s, FMT23, Npumps);
		writeline(s);
		sprintf(s, FMT24, Nvalves);
		writeline(s);
		sprintf(s, FMT25, RptFormTxt[Formflag]);
		writeline(s);
		sprintf(s, FMT26, Hstep * Ucf[TIME], Field[TIME].Units);
		writeline(s);
		sprintf(s, FMT27, Hacc);
		writeline(s);

		sprintf(s, FMT27a, CheckFreq);                                                //(2.00.12 - LR)
		writeline(s);                                                               //(2.00.12 - LR)
		sprintf(s, FMT27b, MaxCheck);                                                 //(2.00.12 - LR)
		writeline(s);                                                               //(2.00.12 - LR)
		sprintf(s, FMT27c, DampLimit);                                                //(2.00.12 - LR)
		writeline(s);                                                               //(2.00.12 - LR)

		sprintf(s, FMT28, MaxIter);
		writeline(s);
		if (Qualflag == NONE || Dur == 0.0)
			sprintf(s, FMT29);
		else if (Qualflag == CHEM)
			sprintf(s, FMT30, ChemName);
		else if (Qualflag == TRACE)
			sprintf(s, FMT31, Node[TraceNode].ID);
		else if (Qualflag == AGE)
			sprintf(s, FMT32);
		writeline(s);
		if (Qualflag != NONE && Dur > 0)
		{
			sprintf(s, FMT33, (float)Qstep / 60.0);
			writeline(s);
			sprintf(s, FMT34, Ctol * Ucf[QUALITY], Field[QUALITY].Units);
			writeline(s);
		}
		sprintf(s, FMT36, SpGrav);
		writeline(s);
		sprintf(s, FMT37a, Viscos / VISCOS);
		writeline(s);
		sprintf(s, FMT37b, Diffus / DIFFUS);
		writeline(s);
		sprintf(s, FMT38, Dmult);
		writeline(s);
		sprintf(s, FMT39, Dur * Ucf[TIME], Field[TIME].Units);
		writeline(s);
		if (Rptflag)
		{
			sprintf(s, FMT40);
			writeline(s);
			if (Nodeflag == 0)  writeline(FMT41);
			if (Nodeflag == 1)  writeline(FMT42);
			if (Nodeflag == 2)  writeline(FMT43);
			writelimits(DEMAND, QUALITY);
			if (Linkflag == 0)  writeline(FMT44);
			if (Linkflag == 1)  writeline(FMT45);
			if (Linkflag == 2)  writeline(FMT46);
			writelimits(DIAM, HEADLOSS);
		}
		writeline(" ");
	}                        /* End of writesummary */


	void EPANET::writehydstat(int iter, double relerr)
		/*
		**--------------------------------------------------------------
		**   Input:   iter   = # iterations to find hydraulic solution
		**            relerr = convergence error in hydraulic solution
		**   Output:  none
		**   Purpose: writes hydraulic status report for solution found
		**            at current time period to report file
		**--------------------------------------------------------------
		*/
	{
		int    i, n;
		char   newstat;
		char   s1[MAXLINE + 1];

		/*** Updated 6/24/02 ***/
		char   atime[13];

		/* Display system status */
		strcpy(atime, clocktime(Atime, Htime));
		if (iter > 0)
		{
			if (relerr <= Hacc)
				sprintf(s1, FMT58, atime, iter);
			else
				sprintf(s1, FMT59, atime, iter, relerr);
			writeline(s1);
		}

		/*
		Display status changes for tanks.
		D[n] is net inflow to tank at node n.
		Old tank status is stored in OldStat[]
		at indexes Nlinks+1 to Nlinks+Ntanks.
		*/
		for (i = 1; i <= Ntanks; i++)
		{
			n = Tank[i].Node;
			if (ABS(D[n]) < 0.001) newstat = CLOSED;
			else if (D[n] > 0.0)  newstat = FILLING;
			else if (D[n] < 0.0)  newstat = EMPTYING;
			else newstat = OldStat[Nlinks + i];
			if (newstat != OldStat[Nlinks + i])
			{
				if (Tank[i].A > 0.0)
					sprintf(s1, FMT50, atime, Node[n].ID, StatTxt[newstat],
						(H[n] - Node[n].El) * Ucf[HEAD], Field[HEAD].Units);
				else sprintf(s1, FMT51, atime, Node[n].ID, StatTxt[newstat]);
				writeline(s1);
				OldStat[Nlinks + i] = newstat;
			}
		}

		/* Display status changes for links */
		for (i = 1; i <= Nlinks; i++)
		{
			if (S[i] != OldStat[i])
			{
				if (Htime == 0)
					sprintf(s1, FMT52, atime, LinkTxt[Link[i].Type], Link[i].ID,
						StatTxt[S[i]]);
				else sprintf(s1, FMT53, atime, LinkTxt[Link[i].Type], Link[i].ID,
					StatTxt[OldStat[i]], StatTxt[S[i]]);
				writeline(s1);
				OldStat[i] = S[i];
			}
		}
		writeline(" ");
	}                        /* End of writehydstat */


	void EPANET::writeenergy()
		/*
		**-------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: writes energy usage report to report file
		**-------------------------------------------------------------
		*/
	{
		int    j;
		double csum;
		char   s[MAXLINE + 1];
		if (Npumps == 0) return;
		writeline(" ");
		writeheader(ENERHDR, 0);
		csum = 0.0;
		for (j = 1; j <= Npumps; j++)
		{
			csum += Pump[j].Energy[5];
			if (LineNum == (long)PageSize) writeheader(ENERHDR, 1);
			sprintf(s, "%-8s  %6.2f %6.2f %9.2f %9.2f %9.2f %9.2f",
				Link[Pump[j].Link].ID, Pump[j].Energy[0], Pump[j].Energy[1],
				Pump[j].Energy[2], Pump[j].Energy[3], Pump[j].Energy[4],
				Pump[j].Energy[5]);
			writeline(s);
		}
		fillstr(s, '-', 63);
		writeline(s);

		/*** Updated 6/24/02 ***/
		sprintf(s, FMT74, "", Emax * Dcost);
		writeline(s);
		sprintf(s, FMT75, "", csum + Emax * Dcost);
		/*** End of update ***/

		writeline(s);
		writeline(" ");
	}                       /* End of writeenergy */


	int EPANET::writeresults()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: writes simulation results to report file
		**--------------------------------------------------------------
		*/
	{
		Pfloat *x;                /* Array of pointers to floats */
		int    j, m, n, np, nnv, nlv;
		int    errcode = 0;

		/*
		**-----------------------------------------------------------
		**  NOTE:  The OutFile contains results for 4 node variables
		**         (demand, head, pressure, & quality) and 8 link
		**         variables (flow, velocity, headloss, quality,
		**         status, setting, reaction rate & friction factor)
		**         at each reporting time.
		**-----------------------------------------------------------
		*/

		/* Return if no output file */
		if (OutFile == NULL) return(106);

		/* Return if no nodes or links selected for reporting */
		/* or if no node or link report variables enabled.    */
		if (!Nodeflag && !Linkflag) return(errcode);
		nnv = 0;
		for (j = ELEV; j <= QUALITY; j++) nnv += Field[j].Enabled;
		nlv = 0;
		for (j = LENGTH; j <= FRICTION; j++) nlv += Field[j].Enabled;
		if (nnv == 0 && nlv == 0) return(errcode);

		/* Allocate memory for output variables. */
		/* m = larger of # node variables & # link variables */
		/* n = larger of # nodes & # links */
		m = MAX((QUALITY - DEMAND + 1), (FRICTION - FLOW + 1));
		n = MAX((Nnodes + 1), (Nlinks + 1));
		x = (Pfloat *)calloc(m, sizeof(Pfloat));
		ERRCODE(MEMCHECK(x));
		if (errcode) return(errcode);
		for (j = 0; j < m; j++)
		{
			x[j] = (REAL4 *)calloc(n, sizeof(REAL4));
			ERRCODE(MEMCHECK(x[j]));
		}
		if (errcode) return(errcode);

		/* Re-position output file & initialize report time. */
		fseek(OutFile, OutOffset2, SEEK_SET);
		Htime = Rstart;

		/* For each reporting time: */
		for (np = 1; np <= Nperiods; np++)
		{

			/* Read in node results & write node table. */
			/* (Remember to offset x[j] by 1 because array is zero-based). */
			for (j = DEMAND; j <= QUALITY; j++)
				fread((x[j - DEMAND]) + 1, sizeof(REAL4), Nnodes, OutFile);
			if (nnv > 0 && Nodeflag > 0) writenodetable(x);

			/* Read in link results & write link table. */
			for (j = FLOW; j <= FRICTION; j++)
				fread((x[j - FLOW]) + 1, sizeof(REAL4), Nlinks, OutFile);
			if (nlv > 0 && Linkflag > 0) writelinktable(x);
			Htime += Rstep;
		}

		/* Free allocated memory */
		for (j = 0; j < m; j++) free(x[j]);
		free(x);
		return(errcode);
	}                        /* End of writereport */


	void EPANET::writenodetable(Pfloat *x)
		/*
		**---------------------------------------------------------------
		**   Input:   x = pointer to node results for current time
		**   Output:  none
		**   Purpose: writes node results for current time to report file
		**---------------------------------------------------------------
		*/
	{
		int    i, j;
		char   s[MAXLINE + 1], s1[16];
		double y[MAXVAR];

		/* Write table header */
		writeheader(NODEHDR, 0);

		/* For each node: */
		for (i = 1; i <= Nnodes; i++)
		{

			/* Place results for each node variable in y */
			y[ELEV] = Node[i].El * Ucf[ELEV];
			for (j = DEMAND; j <= QUALITY; j++) y[j] = *((x[j - DEMAND]) + i);

			/* Check if node gets reported on */
			if ((Nodeflag == 1 || Node[i].Rpt) && checklimits(y, ELEV, QUALITY))
			{

				/* Check if new page needed */
				if (LineNum == (long)PageSize) writeheader(NODEHDR, 1);

				/* Add node ID and each reported field to string s */
				sprintf(s, "%-15s", Node[i].ID);
				for (j = ELEV; j <= QUALITY; j++)
				{
					if (Field[j].Enabled == TRUE)
					{

						/*** Updated 6/24/02 ***/
						if (fabs(y[j]) > 1.e6) sprintf(s1, "%10.2e", y[j]);
						else                   sprintf(s1, "%10.*f", Field[j].Precision, y[j]);
						/*** End of update ***/

						strcat(s, s1);
					}
				}

				/* Note if node is a reservoir/tank */
				if (i > Njuncs)
				{
					strcat(s, "  ");
					strcat(s, NodeTxt[getnodetype(i)]);
				}

				/* Write results for node */
				writeline(s);
			}
		}
		writeline(" ");
	}


	void EPANET::writelinktable(Pfloat *x)
		/*
		**---------------------------------------------------------------
		**   Input:   x = pointer to link results for current time
		**   Output:  none
		**   Purpose: writes link results for current time to report file
		**---------------------------------------------------------------
		*/
	{
		int    i, j, k;
		char   s[MAXLINE + 1], s1[16];
		double y[MAXVAR];

		/* Write table header */
		writeheader(LINKHDR, 0);

		/* For each link: */
		for (i = 1; i <= Nlinks; i++)
		{

			/* Place results for each link variable in y */
			y[LENGTH] = Link[i].Len * Ucf[LENGTH];
			y[DIAM] = Link[i].Diam * Ucf[DIAM];
			for (j = FLOW; j <= FRICTION; j++) y[j] = *((x[j - FLOW]) + i);

			/* Check if link gets reported on */
			if ((Linkflag == 1 || Link[i].Rpt) && checklimits(y, DIAM, FRICTION))
			{

				/* Check if new page needed */
				if (LineNum == (long)PageSize) writeheader(LINKHDR, 1);

				/* Add link ID and each reported field to string s */
				sprintf(s, "%-15s", Link[i].ID);
				for (j = LENGTH; j <= FRICTION; j++)
				{
					if (Field[j].Enabled == TRUE)
					{
						if (j == STATUS)
						{
							if (y[j] <= CLOSED) k = CLOSED;
							else if (y[j] == ACTIVE) k = ACTIVE;
							else                     k = OPEN;
							sprintf(s1, "%10s", StatTxt[k]);
						}

						/*** Updated 6/24/02 ***/
						else
						{
							if (fabs(y[j]) > 1.e6) sprintf(s1, "%10.2e", y[j]);
							else                   sprintf(s1, "%10.*f", Field[j].Precision, y[j]);
						}
						/*** End of update ***/

						strcat(s, s1);
					}
				}

				/* Note if link is a pump or valve */
				if ((j = Link[i].Type) > PIPE)
				{
					strcat(s, "  ");
					strcat(s, LinkTxt[j]);
				}

				/* Write results for link */
				writeline(s);
			}
		}
		writeline(" ");
	}


	void EPANET::writeheader(int type, int contin)
		/*
		**--------------------------------------------------------------
		**   Input:   type   = table type
		**            contin = table continuation flag
		**   Output:  none
		**   Purpose: writes column headings for output report tables
		**--------------------------------------------------------------
		*/
	{
		char   s[MAXLINE + 1], s1[MAXLINE + 1], s2[MAXLINE + 1], s3[MAXLINE + 1];
		int    i, n;

		/* Move to next page if < 11 lines remain on current page. */
		if (Rptflag && LineNum + 11 > (long)PageSize)
		{
			while (LineNum < (long)PageSize) writeline(" ");
		}
		writeline(" ");

		/* Hydraulic Status Table */
		if (type == STATHDR)
		{
			sprintf(s, FMT49);
			if (contin) strcat(s, t_CONTINUED);
			writeline(s);
			fillstr(s, '-', 70);
			writeline(s);
		}

		/* Energy Usage Table */
		if (type == ENERHDR)
		{
			if (Unitsflag == SI) strcpy(s1, t_perM3);
			else                 strcpy(s1, t_perMGAL);
			sprintf(s, FMT71);
			if (contin) strcat(s, t_CONTINUED);
			writeline(s);
			fillstr(s, '-', 63);
			writeline(s);
			sprintf(s, FMT72);
			writeline(s);
			sprintf(s, FMT73, s1);
			writeline(s);
			fillstr(s, '-', 63);
			writeline(s);
		}

		/* Node Results Table */
		if (type == NODEHDR)
		{
			if (Tstatflag == RANGE)  sprintf(s, FMT76, t_DIFFER);
			else if (Tstatflag != SERIES) sprintf(s, FMT76, TstatTxt[Tstatflag]);
			else if (Dur == 0)            sprintf(s, FMT77);
			else                          sprintf(s, FMT78, clocktime(Atime, Htime));
			if (contin) strcat(s, t_CONTINUED);
			writeline(s);
			n = 15;
			sprintf(s2, "%15s", "");
			strcpy(s, t_NODEID);
			sprintf(s3, "%-15s", s);
			for (i = ELEV; i < QUALITY; i++) if (Field[i].Enabled == TRUE)
			{
				n += 10;
				sprintf(s, "%10s", Field[i].Name);
				strcat(s2, s);
				sprintf(s, "%10s", Field[i].Units);
				strcat(s3, s);
			}
			if (Field[QUALITY].Enabled == TRUE)
			{
				n += 10;
				sprintf(s, "%10s", ChemName);
				strcat(s2, s);
				sprintf(s, "%10s", ChemUnits);
				strcat(s3, s);
			}
			fillstr(s1, '-', n);
			writeline(s1);
			writeline(s2);
			writeline(s3);
			writeline(s1);
		}

		/* Link Results Table */
		if (type == LINKHDR)
		{
			if (Tstatflag == RANGE)  sprintf(s, FMT79, t_DIFFER);
			else if (Tstatflag != SERIES) sprintf(s, FMT79, TstatTxt[Tstatflag]);
			else if (Dur == 0)            sprintf(s, FMT80);
			else                          sprintf(s, FMT81, clocktime(Atime, Htime));
			if (contin) strcat(s, t_CONTINUED);
			writeline(s);
			n = 15;
			sprintf(s2, "%15s", "");
			strcpy(s, t_LINKID);
			sprintf(s3, "%-15s", s);
			for (i = LENGTH; i <= FRICTION; i++) if (Field[i].Enabled == TRUE)
			{
				n += 10;
				sprintf(s, "%10s", Field[i].Name);
				strcat(s2, s);
				sprintf(s, "%10s", Field[i].Units);
				strcat(s3, s);
			}
			fillstr(s1, '-', n);
			writeline(s1);
			writeline(s2);
			writeline(s3);
			writeline(s1);
		}
	}                        /* End of writeheader */


	void EPANET::writeline(const char *s)
		/*
		**--------------------------------------------------------------
		**   Input:   *s = text string
		**   Output:  none
		**   Purpose: writes a line of output to report file
		**--------------------------------------------------------------
		*/
	{
		if (RptFile == NULL) return;
		if (Rptflag)
		{
			if (LineNum == (long)PageSize)
			{
				PageNum++;
				if (fprintf(RptFile, FMT82, PageNum, Title[0]) == EOF)
					Fprinterr = TRUE;
				LineNum = 3;
			}
		}
		if (fprintf(RptFile, "\n  %s", s) == EOF) Fprinterr = TRUE;
		LineNum++;
	}                        /* End of writeline */


	void EPANET::writerelerr(int iter, double relerr)
		/*
		**-----------------------------------------------------------------
		**   Input:   iter   = current iteration of hydraulic solution
		**            relerr = current convergence error
		**   Output:  none
		**   Purpose: writes out convergence status of hydraulic solution
		**-----------------------------------------------------------------
		*/
	{
		if (iter == 0)
		{
			sprintf(Msg, FMT64, clocktime(Atime, Htime));
			writeline(Msg);
		}
		else
		{
			sprintf(Msg, FMT65, iter, relerr);
			writeline(Msg);
		}
	}                        /* End of writerelerr */


	void EPANET::writestatchange(int k, char s1, char s2)
		/*
		**--------------------------------------------------------------
		**   Input:   k  = link index
		**            s1 = old link status
		**            s2 = new link status
		**   Output:  none
		**   Purpose: writes change in link status to output report
		**--------------------------------------------------------------
		*/
	{
		int    j1, j2;
		double setting;

		/* We have a pump/valve setting change instead of a status change */
		if (s1 == s2)
		{

			/*** Updated 10/25/00 ***/
			setting = K[k];   //Link[k].Kc;

			switch (Link[k].Type)
			{
			case PRV:
			case PSV:
			case PBV: setting *= Ucf[PRESSURE]; break;
			case FCV: setting *= Ucf[FLOW];
			}
			sprintf(Msg, FMT56, LinkTxt[Link[k].Type], Link[k].ID, setting);
			writeline(Msg);
			return;
		}

		/* We have a status change. Write the old & new status types. */
		if (s1 == ACTIVE) j1 = ACTIVE;
		else if (s1 <= CLOSED) j1 = CLOSED;
		else                   j1 = OPEN;
		if (s2 == ACTIVE) j2 = ACTIVE;
		else if (s2 <= CLOSED) j2 = CLOSED;
		else                   j2 = OPEN;
		if (j1 != j2)
		{
			sprintf(Msg, FMT57, LinkTxt[Link[k].Type], Link[k].ID,
				StatTxt[j1], StatTxt[j2]);
			writeline(Msg);
		}
	}                        /* End of writestatchange */


	void EPANET::writecontrolaction(int k, int i)
		/*
		----------------------------------------------------------------
		**   Input:   k  = link index
		**            i  = control index
		**   Output:  none
		**   Purpose: writes control action taken to status report
		**--------------------------------------------------------------
		*/
	{
		int n;
		switch (Control[i].Type)
		{
		case LOWLEVEL:
		case HILEVEL:
			n = Control[i].Node;
			sprintf(Msg, FMT54, clocktime(Atime, Htime), LinkTxt[Link[k].Type],
				Link[k].ID, NodeTxt[getnodetype(n)], Node[n].ID);
			break;
		case TIMER:
		case TIMEOFDAY:
			sprintf(Msg, FMT55, clocktime(Atime, Htime), LinkTxt[Link[k].Type],
				Link[k].ID);
			break;
		default: return;
		}
		writeline(Msg);
	}


	void EPANET::writeruleaction(int k, char *ruleID)
		/*
		**--------------------------------------------------------------
		**   Input:   k  = link index
		**            *ruleID  = rule ID
		**   Output:  none
		**   Purpose: writes rule action taken to status report
		**--------------------------------------------------------------
		*/
	{
		sprintf(Msg, FMT63, clocktime(Atime, Htime), LinkTxt[Link[k].Type],
			Link[k].ID, ruleID);
		writeline(Msg);
	}


	int EPANET::writehydwarn(int iter, double relerr)
		/*
		**--------------------------------------------------------------
		**   Input:   iter = # iterations to find hydraulic solution
		**   Output:  warning flag code
		**   Purpose: writes hydraulic warning message to report file
		**
		**   Note: Warning conditions checked in following order:
		**         1. System balanced but unstable
		**         2. Negative pressures
		**         3. FCV cannot supply flow or PRV/PSV cannot maintain pressure
		**         4. Pump out of range
		**         5. Network disconnected
		**         6. System unbalanced
		**--------------------------------------------------------------
		*/
	{
		int  i, j;
		char flag = 0;
		char s;                                                                     //(2.00.11 - LR)

																					/* Check if system unstable */
		if (iter > MaxIter && relerr <= Hacc)
		{
			sprintf(Msg, WARN02, clocktime(Atime, Htime));
			if (Messageflag) writeline(Msg);
			flag = 2;
		}

		/* Check for negative pressures */
		for (i = 1; i <= Njuncs; i++)
		{
			if (H[i] < Node[i].El && D[i] > 0.0)
			{
				sprintf(Msg, WARN06, clocktime(Atime, Htime));
				if (Messageflag) writeline(Msg);
				flag = 6;
				break;
			}
		}

		/* Check for abnormal valve condition */
		for (i = 1; i <= Nvalves; i++)
		{
			j = Valve[i].Link;
			if (S[j] >= XFCV)
			{
				sprintf(Msg, WARN05, LinkTxt[Link[j].Type], Link[j].ID,
					StatTxt[S[j]], clocktime(Atime, Htime));
				if (Messageflag) writeline(Msg);
				flag = 5;
			}
		}

		/* Check for abnormal pump condition */
		for (i = 1; i <= Npumps; i++)
		{
			j = Pump[i].Link;
			s = S[j];                                                                //(2.00.11 - LR)
			if (S[j] >= OPEN)                                                        //(2.00.11 - LR)
			{                                                                        //(2.00.11 - LR)
				if (Q[j] > K[j] * Pump[i].Qmax) s = XFLOW;                             //(2.00.11 - LR)
				if (Q[j] < 0.0) s = XHEAD;                                           //(2.00.11 - LR)
			}                                                                        //(2.00.11 - LR)
			if (s == XHEAD || s == XFLOW)                                            //(2.00.11 - LR)
			{
				sprintf(Msg, WARN04, Link[j].ID, StatTxt[s],                             //(2.00.11 - LR)
					clocktime(Atime, Htime));
				if (Messageflag) writeline(Msg);
				flag = 4;
			}
		}

		/* Check if system is unbalanced */
		if (iter > MaxIter && relerr > Hacc)
		{
			sprintf(Msg, WARN01, clocktime(Atime, Htime));
			if (ExtraIter == -1) strcat(Msg, t_HALTED);
			if (Messageflag) writeline(Msg);
			flag = 1;
		}

		/* Check for disconnected network */
		/* & update global warning flag   */
		if (flag > 0)
		{
			disconnected();
			Warnflag = flag;
		}
		return(flag);
	}                        /* End of writehydwarn */


	void EPANET::writehyderr(int errnode)
		/*
		**-----------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: outputs status & checks connectivity when
		**            network hydraulic equations cannot be solved.
		**-----------------------------------------------------------
		*/
	{
		sprintf(Msg, FMT62, clocktime(Atime, Htime), Node[errnode].ID);
		if (Messageflag) writeline(Msg);
		writehydstat(0, 0);
		disconnected();
	}                        /* End of writehyderr */


	int EPANET::disconnected()
		/*
		**-------------------------------------------------------------------
		**   Input:   None
		**   Output:  Returns number of disconnected nodes
		**   Purpose: Tests current hydraulic solution to see if any closed
		**            links have caused the network to become disconnected.
		**-------------------------------------------------------------------
		*/
	{
		int  i, j;
		int  count, mcount;
		int  errcode = 0;
		int *nodelist;
		char *marked;

		/* Allocate memory for node list & marked list */
		nodelist = (int *)calloc(Nnodes + 1, sizeof(int));
		marked = (char *)calloc(Nnodes + 1, sizeof(char));
		ERRCODE(MEMCHECK(nodelist));
		ERRCODE(MEMCHECK(marked));
		if (errcode) return(0);

		/* Place tanks on node list and marked list */
		for (i = 1; i <= Ntanks; i++)
		{
			j = Njuncs + i;
			nodelist[i] = j;
			marked[j] = 1;
		}

		/* Place junctions with negative demands on the lists */
		mcount = Ntanks;
		for (i = 1; i <= Njuncs; i++)
		{
			if (D[i] < 0.0)
			{
				mcount++;
				nodelist[mcount] = i;
				marked[i] = 1;
			}
		}

		/* Mark all nodes that can be connected to tanks */
		/* and count number of nodes remaining unmarked. */
		marknodes(mcount, nodelist, marked);
		j = 0;
		count = 0;
		for (i = 1; i <= Njuncs; i++)
		{
			if (!marked[i] && D[i] != 0.0)  /* Skip if no demand */
			{
				count++;
				if (count <= MAXCOUNT && Messageflag)
				{
					sprintf(Msg, WARN03a, Node[i].ID, clocktime(Atime, Htime));
					writeline(Msg);
				}
				j = i;                       /* Last unmarked node */
			}
		}

		/* Report number of unmarked nodes and find closed link */
		/* on path from node j back to a tank.                  */
		if (count > 0 && Messageflag)
		{
			if (count > MAXCOUNT)
			{
				sprintf(Msg, WARN03b, count - MAXCOUNT, clocktime(Atime, Htime));
				writeline(Msg);
			}
			getclosedlink(j, marked);
		}

		/* Free allocated memory */
		free(nodelist);
		free(marked);
		return(count);
	}                   /* End of disconnected() */


	void EPANET::marknodes(int m, int *nodelist, char *marked)
		/*
		**----------------------------------------------------------------
		**   Input:   m = number of source nodes
		**            nodelist[] = list of nodes to be traced from
		**            marked[]   = TRUE if node connected to source
		**   Output:  None.
		**   Purpose: Marks all junction nodes connected to tanks.
		**----------------------------------------------------------------
		*/
	{
		int   i, j, k, n;
		Padjlist alink;

		/* Scan each successive entry of node list */
		n = 1;
		while (n <= m)
		{

			/* Scan all nodes connected to current node */
			i = nodelist[n];
			for (alink = Adjlist[i]; alink != NULL; alink = alink->next)
			{

				/* Get indexes of connecting link and node */
				k = alink->link;
				j = alink->node;
				if (marked[j]) continue;

				/* Check if valve connection is in correct direction */
				switch (Link[k].Type)
				{
				case CV:
				case PRV:
				case PSV: if (j == Link[k].N1) continue;
				}

				/* Mark connection node if link not closed */
				if (S[k] > CLOSED)
				{
					marked[j] = 1;
					m++;
					nodelist[m] = j;
				}
			}
			n++;
		}
	}                   /* End of marknodes() */


	void EPANET::getclosedlink(int i, char *marked)
		/*
		**----------------------------------------------------------------
		**   Input:   i = junction index
		**            marked[] = marks nodes already examined
		**   Output:  None.
		**   Purpose: Determines if a closed link connects to junction i.
		**----------------------------------------------------------------
		*/
	{
		int j, k;
		Padjlist alink;
		marked[i] = 2;
		for (alink = Adjlist[i]; alink != NULL; alink = alink->next)
		{
			k = alink->link;
			j = alink->node;
			if (marked[j] == 2) continue;
			if (marked[j] == 1)
			{
				sprintf(Msg, WARN03c, Link[k].ID);
				writeline(Msg);
				return;
			}
			else getclosedlink(j, marked);
		}
	}


	void  EPANET::writelimits(int j1, int j2)
		/*
		**--------------------------------------------------------------
		**   Input:   j1 = index of first output variable
		**            j2 = index of last output variable
		**   Output:  none
		**   Purpose: writes reporting criteria to output report
		**--------------------------------------------------------------
		*/
	{
		int  j;
		for (j = j1; j <= j2; j++)
		{
			if (Field[j].RptLim[LOW] < BIG)
			{
				sprintf(Msg, FMT47,
					Field[j].Name, Field[j].RptLim[LOW], Field[j].Units);
				writeline(Msg);
			}
			if (Field[j].RptLim[HI] > -BIG)
			{
				sprintf(Msg, FMT48,
					Field[j].Name, Field[j].RptLim[HI], Field[j].Units);
				writeline(Msg);
			}
		}
	}                        /* End of writelimits */


	int  EPANET::checklimits(double *y, int j1, int j2)
		/*
		**--------------------------------------------------------------
		**   Input:   *y = array of output results
		**            j1 = index of first output variable
		**            j2 = index of last output variable
		**   Output:  returns 1 if criteria met, 0 otherwise
		**   Purpose: checks if output reporting criteria is met
		**--------------------------------------------------------------
		*/
	{
		int j;
		for (j = j1; j <= j2; j++)
		{
			if (y[j] > Field[j].RptLim[LOW]
				|| y[j] < Field[j].RptLim[HI]) return(0);
		}
		return(1);
	}                        /* End of checklim */


	void EPANET::writetime(const char *fmt)
		/*
		**----------------------------------------------------------------
		**   Input:   fmt = format string
		**   Output:  none
		**   Purpose: writes starting/ending time of a run to report file
		**----------------------------------------------------------------
		*/
	{
		time_t timer;
		time(&timer);
		sprintf(Msg, fmt, ctime(&timer));
		writeline(Msg);
	}


	char *EPANET::clocktime(char *atime, long seconds)
		/*
		**--------------------------------------------------------------
		**   Input:   seconds = time in seconds
		**   Output:  atime = time in hrs:min
		**            (returns pointer to atime)
		**   Purpose: converts time in seconds to hours:minutes format
		**--------------------------------------------------------------
		*/
	{
		/*** Updated 6/24/02 ***/
		int h, m, s;
		h = seconds / 3600;
		m = (seconds % 3600) / 60;
		s = seconds - 3600 * h - 60 * m;
		sprintf(atime, "%01d:%02d:%02d", h, m, s);
		return(atime);
	}                        /* End of clocktime */


	char *EPANET::fillstr(char *s, char ch, int n)
		/*
		**---------------------------------------------------------
		**  Fills n bytes of s to character ch.
		**  NOTE: does not check for overwriting s.
		**---------------------------------------------------------
		*/
	{
		int i;
		for (i = 0; i <= n; i++) s[i] = ch;
		s[n + 1] = '\0';
		return(s);
	}


	int  EPANET::getnodetype(int i)
		/*
		**---------------------------------------------------------
		**  Determines type of node with index i
		**  (junction = 0, reservoir = 1, tank = 2).
		**---------------------------------------------------------
		*/
	{
		if (i <= Njuncs) return(0);
		if (Tank[i - Njuncs].A == 0.0) return(1);
		return(2);
	}

	/*hydraul*/
	int  EPANET::openhyd()
		/*
		*--------------------------------------------------------------
		*  Input:   none
		*  Output:  returns error code
		*  Purpose: opens hydraulics solver system
		*--------------------------------------------------------------
		*/
	{
		int  i;
		int  errcode = 0;
		ERRCODE(createsparse());     /* See SMATRIX.C  */
		ERRCODE(allocmatrix());      /* Allocate solution matrices */
		for (i = 1; i <= Nlinks; i++)    /* Initialize flows */
			initlinkflow(i, Link[i].Stat, Link[i].Kc);
		return(errcode);
	}


	/*** Updated 3/1/01 ***/
	void EPANET::inithyd(int initflag)
		/*
		**--------------------------------------------------------------
		**  Input:   initflag > 0 if link flows should be re-initialized
		**                    = 0 if not
		**  Output:  none
		**  Purpose: initializes hydraulics solver system
		**--------------------------------------------------------------
		*/
	{
		int i, j;

		/* Initialize tanks */
		for (i = 1; i <= Ntanks; i++)
		{
			Tank[i].V = Tank[i].V0;
			H[Tank[i].Node] = Tank[i].H0;

			/*** Updated 10/25/00 ***/
			D[Tank[i].Node] = 0.0;

			OldStat[Nlinks + i] = TEMPCLOSED;
		}

		/* Initialize emitter flows */
		memset(E, 0, (Nnodes + 1) * sizeof(double));
		for (i = 1; i <= Njuncs; i++)
			if (Node[i].Ke > 0.0) E[i] = 1.0;

		/* Initialize links */
		for (i = 1; i <= Nlinks; i++)
		{
			/* Initialize status and setting */
			S[i] = Link[i].Stat;
			K[i] = Link[i].Kc;

			/* Start active control valves in ACTIVE position */                     //(2.00.11 - LR)
			if (
				(Link[i].Type == PRV || Link[i].Type == PSV
					|| Link[i].Type == FCV)                                            //(2.00.11 - LR)
				&& (Link[i].Kc != MISSING)
				) S[i] = ACTIVE;                                                      //(2.00.11 - LR)

																					  /*** Updated 3/1/01 ***/
																					  /* Initialize flows if necessary */
			if (S[i] <= CLOSED) Q[i] = QZERO;
			else if (ABS(Q[i]) <= QZERO || initflag > 0)
				initlinkflow(i, S[i], K[i]);

			/* Save initial status */
			OldStat[i] = S[i];
		}

		/* Reset pump energy usage */
		for (i = 1; i <= Npumps; i++)
		{
			for (j = 0; j < 6; j++) Pump[i].Energy[j] = 0.0;
		}

		/* Re-position hydraulics file */
		if (Saveflag) fseek(HydFile, HydOffset, SEEK_SET);

		/*** Updated 3/1/01 ***/
		/* Initialize current time */
		Haltflag = 0;
		Htime = 0;
		Hydstep = 0;
		Rtime = Rstep;
	}


	int   EPANET::runhyd(long *t)
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  t = pointer to current time (in seconds)
		**  Returns: error code
		**  Purpose: solves network hydraulics in a single time period
		**--------------------------------------------------------------
		*/
	{
		int   iter;                          /* Iteration count   */
		int   errcode;                       /* Error code        */
		double relerr;                        /* Solution accuracy */

											  /* Find new demands & control actions */
		*t = Htime;
		demands();
		controls();

		/* Solve network hydraulic equations */
		errcode = netsolve(&iter, &relerr);
		if (!errcode)
		{
			/* Report new status & save results */
			if (Statflag) writehydstat(iter, relerr);

			/*** Updated 3/1/01 ***/
			/* If system unbalanced and no extra trials */
			/* allowed, then activate the Haltflag.     */
			if (relerr > Hacc && ExtraIter == -1) Haltflag = 1;

			/* Report any warning conditions */
			if (!errcode) errcode = writehydwarn(iter, relerr);
		}
		return(errcode);
	}                               /* end of runhyd */


	int  EPANET::nexthyd(long *tstep)
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  tstep = pointer to time Step (in seconds)
		**  Returns: error code
		**  Purpose: finds length of next time Step & updates tank
		**           levels and rule-based contol actions
		**--------------------------------------------------------------
		*/
	{
		long  hydstep;         /* Actual time Step  */
		int   errcode = 0;     /* Error code        */

							   /*** Updated 3/1/01 ***/
							   /* Save current results to hydraulics file and */
							   /* force end of simulation if Haltflag is active */
		if (Saveflag) errcode = savehyd(&Htime);
		if (Haltflag) Htime = Dur;

		/* Compute next time Step & update tank levels */
		*tstep = 0;
		hydstep = 0;
		if (Htime < Dur) hydstep = timestep();
		if (Saveflag) errcode = savehydstep(&hydstep);

		/* Compute pumping energy */
		if (Dur == 0) addenergy(0);
		else if (Htime < Dur) addenergy(hydstep);

		/* Update current time. */
		if (Htime < Dur)  /* More time remains */
		{
			Htime += hydstep;
			if (Htime >= Rtime) Rtime += Rstep;
		}
		else
		{
			Htime++;          /* Force completion of analysis */
		}
		*tstep = hydstep;
		return(errcode);
	}


	void  EPANET::closehyd()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: closes hydraulics solver system
		**--------------------------------------------------------------
		*/
	{
		freesparse();           /* see SMATRIX.C */
		freematrix();
	}


	int  EPANET::allocmatrix()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: allocates memory used for solution matrix coeffs.
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;
		Aii = (double *)calloc(Nnodes + 1, sizeof(double));
		Aij = (double *)calloc(Ncoeffs + 1, sizeof(double));
		F = (double *)calloc(Nnodes + 1, sizeof(double));
		E = (double *)calloc(Nnodes + 1, sizeof(double));
		P = (double *)calloc(Nlinks + 1, sizeof(double));
		Y = (double *)calloc(Nlinks + 1, sizeof(double));
		X = (double *)calloc(MAX((Nnodes + 1), (Nlinks + 1)), sizeof(double));
		OldStat = (char *)calloc(Nlinks + Ntanks + 1, sizeof(char));
		ERRCODE(MEMCHECK(Aii));
		ERRCODE(MEMCHECK(Aij));
		ERRCODE(MEMCHECK(F));
		ERRCODE(MEMCHECK(E));
		ERRCODE(MEMCHECK(P));
		ERRCODE(MEMCHECK(Y));
		ERRCODE(MEMCHECK(X));
		ERRCODE(MEMCHECK(OldStat));
		return(errcode);
	}                               /* end of allocmatrix */


	void  EPANET::freematrix()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: frees memory used for solution matrix coeffs.
		**--------------------------------------------------------------
		*/
	{
		free(Aii);
		free(Aij);
		free(F);
		free(E);
		free(P);
		free(Y);
		free(X);
		free(OldStat);
	}                               /* end of freematrix */


	void  EPANET::initlinkflow(int i, char s, double k)
		/*
		**--------------------------------------------------------------------
		**  Input:   i = link index
		**           s = link status
		**           k = link setting (i.e., pump speed)
		**  Output:  none
		**  Purpose: sets initial flow in link to QZERO if link is closed,
		**           to design flow for a pump, or to flow at velocity of
		**           1 fps for other links.
		**--------------------------------------------------------------------
		*/
	{
		if (s == CLOSED) Q[i] = QZERO;
		else if (Link[i].Type == PUMP) Q[i] = k * Pump[PUMPINDEX(i)].Q0;
		else Q[i] = PI * SQR(Link[i].Diam) / 4.0;
	}


	/*** Updated 9/7/00 ***/
	void  EPANET::setlinkflow(int k, double dh)
		/*
		**--------------------------------------------------------------
		**  Input:   k = link index
		**           dh = headloss across link
		**  Output:  none
		**  Purpose: sets flow in link based on current headloss
		**--------------------------------------------------------------
		*/
	{
		int   i, p;
		double h0;
		double  x, y;

		switch (Link[k].Type)
		{
		case CV:
		case PIPE:

			/* For Darcy-Weisbach formula: */
			/* use approx. inverse of formula. */
			if (Formflag == DW)
			{
				x = -log(K[k] / 3.7 / Link[k].Diam);
				y = sqrt(ABS(dh) / Link[k].R / 1.32547);
				Q[k] = x * y;
			}

			/* For Hazen-Williams or Manning formulas: */
			/* use inverse of formula. */
			else
			{
				x = ABS(dh) / Link[k].R;
				y = 1.0 / Hexp;
				Q[k] = pow(x, y);
			}

			/* Change sign of flow to match sign of headloss */
			if (dh < 0.0) Q[k] = -Q[k];
			break;

		case PUMP:

			/* Convert headloss to pump head gain */
			dh = -dh;
			p = PUMPINDEX(k);

			/* For custom pump curve, interpolate from curve */
			if (Pump[p].Ptype == CUSTOM)
			{
				dh = -dh * Ucf[HEAD] / SQR(K[k]);
				i = Pump[p].Hcurve;
				Q[k] = interp(Curve[i].Npts, Curve[i].Y, Curve[i].X,
					dh) * K[k] / Ucf[FLOW];
			}

			/* Otherwise use inverse of power curve */
			else
			{
				h0 = -SQR(K[k]) * Pump[p].H0;
				x = pow(K[k], 2.0 - Pump[p].N);
				x = ABS(h0 - dh) / (Pump[p].R * x),
					y = 1.0 / Pump[p].N;
				Q[k] = pow(x, y);
			}
			break;
		}
	}


	void  EPANET::setlinkstatus(int index, char value, char *s, double *k)
		/*----------------------------------------------------------------
		**  Input:   index  = link index
		**           value  = 0 (CLOSED) or 1 (OPEN)
		**           s      = pointer to link status
		**           k      = pointer to link setting
		**  Output:  none
		**  Purpose: sets link status to OPEN or CLOSED
		**----------------------------------------------------------------
		*/
	{
		/* Status set to open */
		if (value == 1)
		{
			/* Adjust link setting for pumps & valves */
			if (Link[index].Type == PUMP) *k = 1.0;

			/*** Updated 9/7/00 ***/
			if (Link[index].Type > PUMP
				&& Link[index].Type != GPV) *k = MISSING;

			/* Reset link flow if it was originally closed */
			//      if (*s <= CLOSED) initlinkflow(index, OPEN, *k);
			*s = OPEN;
		}

		/* Status set to closed */
		else if (value == 0)
		{
			/* Adjust link setting for pumps & valves */
			if (Link[index].Type == PUMP) *k = 0.0;

			/*** Updated 9/7/00 ***/
			if (Link[index].Type > PUMP
				&& Link[index].Type != GPV) *k = MISSING;

			/* Reset link flow if it was originally open */
			//      if (*s > CLOSED) initlinkflow(index, CLOSED, *k);
			*s = CLOSED;
		}
	}


	void  EPANET::setlinksetting(int index, double value, char *s, double *k)
		/*----------------------------------------------------------------
		**  Input:   index  = link index
		**           value  = pump speed or valve setting
		**           s      = pointer to link status
		**           k      = pointer to link setting
		**  Output:  none
		**  Purpose: sets pump speed or valve setting, adjusting link
		**           status and flow when necessary
		**----------------------------------------------------------------
		*/
	{
		/* For a pump, status is OPEN if speed > 0, CLOSED otherwise */
		if (Link[index].Type == PUMP)
		{
			*k = value;
			if (value > 0 && *s <= CLOSED)
			{
				*s = OPEN;
				//         initlinkflow(index, OPEN, value);
			}
			if (value == 0 && *s > CLOSED)
			{
				*s = CLOSED;
				//          initlinkflow(index, CLOSED, value);
			}
		}

		/***  Updated 9/7/00  ***/
		/* For FCV, activate it */
		else if (Link[index].Type == FCV)
		{
			//      if (*s <= CLOSED) initlinkflow(index, OPEN, value);
			*k = value;
			*s = ACTIVE;
		}

		/* Open closed control valve with fixed status (setting = MISSING) */
		else
		{
			if (*k == MISSING && *s <= CLOSED)
			{
				//         initlinkflow(index, OPEN, value);
				*s = OPEN;
			}
			*k = value;
		}
	}


	void  EPANET::resistance(int k)
		/*
		**--------------------------------------------------------------------
		**  Input:   k = link index
		**  Output:  none
		**  Purpose: computes link flow resistance
		**--------------------------------------------------------------------
		*/
	{
		double e, d, L;
		Link[k].R = CSMALL;
		//if (Link[k].Type == PIPE || Link[k].Type == CV)                           //(2.00.11 - LR)
		switch (Link[k].Type)
		{

			/* Link is a pipe. Compute resistance based on headloss formula. */
			/* Friction factor for D-W formula gets included during solution */
			/* process in pipecoeff() function.                              */
		case CV:
		case PIPE:
			e = Link[k].Kc;                 /* Roughness coeff. */
			d = Link[k].Diam;               /* Diameter */
			L = Link[k].Len;                /* Length */
			switch (Formflag)
			{
			case HW: Link[k].R = 4.727 * L / pow(e, Hexp) / pow(d, 4.871);
				break;
			case DW: Link[k].R = L / 2.0 / 32.2 / d / SQR(PI * SQR(d) / 4.0);
				break;
			case CM: Link[k].R = SQR(4.0 * e / (1.49 * PI * d * d)) *
				pow((d / 4.0), -1.333) * L;
			}
			break;

			/* Link is a pump. Use negligible resistance. */
		case PUMP:
			Link[k].R = CBIG;  //CSMALL;
			break;


			/* Link is a valve. Compute resistance for open valve assuming  */
			/* length is 2*diameter and friction factor is 0.02. Use with   */
			/* other formulas as well since resistance should be negligible.*/

			/*** This way of treating valve resistance has been deprecated  ***/           //(2.00.11 - LR)
																						   /*** since resulting resistance is not always negligible.       ***/           //(2.00.11 - LR)
																																										  /*
																																										  default:
																																										  d = Link[k].Diam;
																																										  L = 2.0*d;
																																										  Link[k].R = 0.02*L/2.0/32.2/d/SQR(PI*SQR(d)/4.0);
																																										  break;
																																										  */
		}
	}


	void EPANET::demands()
		/*
		**--------------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: computes demands at nodes during current time period
		**--------------------------------------------------------------------
		*/
	{
		int i, j, n;
		long k, p;
		double djunc, sum;
		Pdemand demand;

		/* Determine total elapsed number of pattern periods */
		p = (Htime + Pstart) / Pstep;

		/* Update demand at each node according to its assigned pattern */
		Dsystem = 0.0;          /* System-wide demand */
		for (i = 1; i <= Njuncs; i++)
		{
			sum = 0.0;
			for (demand = Node[i].D; demand != NULL; demand = demand->next)
			{
				/*
				pattern period (k) = (elapsed periods) modulus
				(periods per pattern)
				*/
				j = demand->Pat;
				k = p % (long)Pattern[j].Length;
				djunc = (demand->Base) * Pattern[j].F[k] * Dmult;
				if (djunc > 0.0) Dsystem += djunc;
				sum += djunc;
			}
			D[i] = sum;
		}

		/* Update head at fixed grade nodes with time patterns. */
		for (n = 1; n <= Ntanks; n++)
		{
			if (Tank[n].A == 0.0)
			{
				j = Tank[n].Pat;
				if (j > 0)
				{
					k = p % (long)Pattern[j].Length;
					i = Tank[n].Node;
					H[i] = Node[i].El * Pattern[j].F[k];
				}
			}
		}

		/* Update status of pumps with utilization patterns */
		for (n = 1; n <= Npumps; n++)
		{
			j = Pump[n].Upat;
			if (j > 0)
			{
				i = Pump[n].Link;
				k = p % (long)Pattern[j].Length;
				setlinksetting(i, Pattern[j].F[k], &S[i], &K[i]);
			}
		}
	}                        /* End of demands */


	int  EPANET::controls()
		/*
		**---------------------------------------------------------------------
		**  Input:   none
		**  Output:  number of links whose setting changes
		**  Purpose: implements simple controls based on time or tank levels
		**---------------------------------------------------------------------
		*/
	{
		int   i, k, n, reset, setsum;
		double h, vplus;
		double v1, v2;
		double k1, k2;
		char  s1, s2;

		/* Examine each control statement */
		setsum = 0;
		for (i = 1; i <= Ncontrols; i++)
		{
			/* Make sure that link is defined */
			reset = 0;
			if ((k = Control[i].Link) <= 0) continue;

			/* Link is controlled by tank level */
			if ((n = Control[i].Node) > 0 && n > Njuncs)
			{
				h = H[n];
				vplus = ABS(D[n]);
				v1 = tankvolume(n - Njuncs, h);
				v2 = tankvolume(n - Njuncs, Control[i].Grade);
				if (Control[i].Type == LOWLEVEL && v1 <= v2 + vplus)
					reset = 1;
				if (Control[i].Type == HILEVEL && v1 >= v2 - vplus)
					reset = 1;
			}

			/* Link is time-controlled */
			if (Control[i].Type == TIMER)
			{
				if (Control[i].Time == Htime) reset = 1;
			}

			/* Link is time-of-day controlled */
			if (Control[i].Type == TIMEOFDAY)
			{
				if ((Htime + Tstart) % SECperDAY == Control[i].Time) reset = 1;
			}

			/* Update link status & pump speed or valve setting */
			if (reset == 1)
			{
				if (S[k] <= CLOSED) s1 = CLOSED;
				else                s1 = OPEN;
				s2 = Control[i].Status;
				k1 = K[k];
				k2 = k1;
				if (Link[k].Type > PIPE) k2 = Control[i].Setting;
				if (s1 != s2 || k1 != k2)
				{
					S[k] = s2;
					K[k] = k2;
					if (Statflag) writecontrolaction(k, i);
					//           if (s1 != s2) initlinkflow(k, S[k], K[k]);
					setsum++;
				}
			}
		}
		return(setsum);
	}                        /* End of controls */


	long  EPANET::timestep(void)
		/*
		**----------------------------------------------------------------
		**  Input:   none
		**  Output:  returns time Step until next change in hydraulics
		**  Purpose: computes time Step to advance hydraulic simulation
		**----------------------------------------------------------------
		*/
	{
		long   n, t, tstep;

		/* Normal time Step is hydraulic time Step */
		tstep = Hstep;

		/* Revise time Step based on time until next demand period */
		n = ((Htime + Pstart) / Pstep) + 1;   /* Next pattern period   */
		t = n * Pstep - Htime;              /* Time till next period */
		if (t > 0 && t < tstep) tstep = t;

		/* Revise time Step based on time until next reporting period */
		t = Rtime - Htime;
		if (t > 0 && t < tstep) tstep = t;

		/* Revise time Step based on smallest time to fill or drain a tank */
		tanktimestep(&tstep);

		/* Revise time Step based on smallest time to activate a control */
		controltimestep(&tstep);

		/* Evaluate rule-based controls (which will also update tank levels) */
		if (Nrules > 0) ruletimestep(&tstep);
		else tanklevels(tstep);
		return(tstep);
	}


	void EPANET::tanktimestep(long *tstep)
		/*
		**-----------------------------------------------------------------
		**  Input:   *tstep = current time Step
		**  Output:  *tstep = modified current time Step
		**  Purpose: revises time Step based on shortest time to fill or
		**           drain a tank
		**-----------------------------------------------------------------
		*/
	{
		int    i, n;
		double  h, q, v;
		long   t;

		/* (D[n] is net flow rate into (+) or out of (-) tank at node n) */
		for (i = 1; i <= Ntanks; i++)
		{
			if (Tank[i].A == 0.0) continue;           /* Skip reservoirs     */
			n = Tank[i].Node;
			h = H[n];                                 /* Current tank grade  */
			q = D[n];                                 /* Flow into tank      */
			if (ABS(q) <= QZERO) continue;
			if (q > 0.0 && h < Tank[i].Hmax)
			{
				v = Tank[i].Vmax - Tank[i].V;          /* Volume to fill      */
			}
			else if (q < 0.0 && h > Tank[i].Hmin)
			{
				v = Tank[i].Vmin - Tank[i].V;          /* Volume to drain (-) */
			}
			else continue;
			t = (long)ROUND(v / q);                     /* Time to fill/drain  */
			if (t > 0 && t < *tstep) *tstep = t;
		}
	}


	void EPANET::controltimestep(long *tstep)
		/*
		**------------------------------------------------------------------
		**  Input:   *tstep = current time Step
		**  Output:  *tstep = modified current time Step
		**  Purpose: revises time Step based on shortest time to activate
		**           a simple control
		**------------------------------------------------------------------
		*/
	{
		int   i, j, k, n;
		double h, q, v;
		long  t, t1, t2;

		for (i = 1; i <= Ncontrols; i++)
		{
			t = 0;
			if ((n = Control[i].Node) > 0)           /* Node control:       */
			{
				if ((j = n - Njuncs) <= 0) continue;     /* Node is a tank      */
				h = H[n];                              /* Current tank grade  */
				q = D[n];                              /* Flow into tank      */
				if (ABS(q) <= QZERO) continue;
				if
					((h < Control[i].Grade &&
						Control[i].Type == HILEVEL &&       /* Tank below hi level */
						q > 0.0)                            /* & is filling        */
						|| (h > Control[i].Grade &&
							Control[i].Type == LOWLEVEL &&     /* Tank above low level */
							q < 0.0)                           /* & is emptying        */
						)
				{                                      /* Time to reach level  */
					v = tankvolume(j, Control[i].Grade) - Tank[j].V;
					t = (long)ROUND(v / q);
				}
			}

			if (Control[i].Type == TIMER)             /* Time control:        */
			{
				if (Control[i].Time > Htime)
					t = Control[i].Time - Htime;
			}

			if (Control[i].Type == TIMEOFDAY)         /* Time-of-day control: */
			{
				t1 = (Htime + Tstart) % SECperDAY;
				t2 = Control[i].Time;
				if (t2 >= t1) t = t2 - t1;
				else t = SECperDAY - t1 + t2;
			}

			if (t > 0 && t < *tstep)               /* Revise time Step     */
			{
				/* Check if rule actually changes link status or setting */
				k = Control[i].Link;
				if (
					(Link[k].Type > PIPE && K[k] != Control[i].Setting) ||
					(S[k] != Control[i].Status)
					)
					*tstep = t;
			}
		}
	}                        /* End of timestep */


	void  EPANET::ruletimestep(long *tstep)
		/*
		**--------------------------------------------------------------
		**  Input:   *tstep = current time Step (sec)
		**  Output:  *tstep = modified time Step
		**  Purpose: updates next time Step by checking if any rules
		**           will fire before then; also updates tank levels.
		**--------------------------------------------------------------
		*/
	{
		long tnow,      /* Start of time interval for rule evaluation */
			tmax,      /* End of time interval for rule evaluation   */
			dt,        /* Normal time increment for rule evaluation  */
			dt1;       /* Actual time increment for rule evaluation  */

					   /* Find interval of time for rule evaluation */
		tnow = Htime;
		tmax = tnow + *tstep;

		/* If no rules, then time increment equals current time Step */
		if (Nrules == 0)
		{
			dt = *tstep;
			dt1 = dt;
		}

		/* Otherwise, time increment equals rule evaluation time Step and */
		/* first actual increment equals time until next even multiple of */
		/* Rulestep occurs. */
		else
		{
			dt = Rulestep;
			dt1 = Rulestep - (tnow % Rulestep);
		}

		/* Make sure time increment is no larger than current time Step */
		dt = MIN(dt, *tstep);
		dt1 = MIN(dt1, *tstep);
		if (dt1 == 0) dt1 = dt;

		/* Step through time, updating tank levels, until either  */
		/* a rule fires or we reach the end of evaluation period. */
		/*
		** Note: we are updating the global simulation time (Htime)
		**       here because it is used by functions in RULES.C
		**       to evaluate rules when checkrules() is called.
		**       It is restored to its original value after the
		**       rule evaluation process is completed (see below).
		**       Also note that dt1 will equal dt after the first
		**       time increment is taken.
		*/
		do
		{
			Htime += dt1;               /* Update simulation clock */
			tanklevels(dt1);            /* Find new tank levels    */
			if (checkrules(dt1)) break; /* Stop if rules fire      */
			dt = MIN(dt, tmax - Htime); /* Update time increment   */
			dt1 = dt;                   /* Update actual increment */
		} while (dt > 0);             /* Stop if no time left    */

									  /* Compute an updated simulation time Step (*tstep) */
									  /* and return simulation time to its original value */
		*tstep = Htime - tnow;
		Htime = tnow;
	}


	void  EPANET::addenergy(long hstep)
		/*
		**-------------------------------------------------------------
		**  Input:   hstep = time Step (sec)
		**  Output:  none
		**  Purpose: accumulates pump energy usage
		**-------------------------------------------------------------
		*/
	{
		int   i, j, k;
		long  m, n;
		double c0, c,             /* Energy cost (cost/kwh) */
			f0,               /* Energy cost factor */
			dt,               /* Time interval (hr) */
			e,                /* Pump efficiency (fraction) */
			q,                /* Pump flow (cfs) */
			p,                /* Pump energy (kw) */
			psum = 0.0;       /* Total energy (kw) */

							  /* Determine current time interval in hours */
		if (Dur == 0)    dt = 1.0;
		else if (Htime < Dur) dt = (double)hstep / 3600.0;
		else                  dt = 0.0;
		if (dt == 0.0)   return;
		n = (Htime + Pstart) / Pstep;

		/* Compute default energy cost at current time */
		c0 = Ecost;
		f0 = 1.0;
		if (Epat > 0)
		{
			m = n % (long)Pattern[Epat].Length;
			f0 = Pattern[Epat].F[m];
		}

		/* Examine each pump */
		for (j = 1; j <= Npumps; j++)
		{
			/* Skip closed pumps */
			k = Pump[j].Link;
			if (S[k] <= CLOSED) continue;
			q = MAX(QZERO, ABS(Q[k]));

			/* Find pump-specific energy cost */
			if (Pump[j].Ecost > 0.0) c = Pump[j].Ecost;
			else c = c0;

			if ((i = Pump[j].Epat) > 0)
			{
				m = n % (long)Pattern[i].Length;
				c *= Pattern[i].F[m];
			}
			else c *= f0;

			/* Find pump energy & efficiency */
			getenergy(k, &p, &e);
			psum += p;

			/* Update pump's cumulative statistics */
			Pump[j].Energy[0] += dt;            /* Time on-line */
			Pump[j].Energy[1] += e * dt;          /* Effic.-hrs   */
			Pump[j].Energy[2] += p / q * dt;        /* kw/cfs-hrs   */
			Pump[j].Energy[3] += p * dt;          /* kw-hrs       */
			Pump[j].Energy[4] = MAX(Pump[j].Energy[4], p);
			Pump[j].Energy[5] += c * p * dt;        /* cost-hrs.    */
		}

		/* Update maximum kw value */
		Emax = MAX(Emax, psum);
	}                       /* End of pumpenergy */


	void  EPANET::getenergy(int k, double *kw, double *eff)
		/*
		**----------------------------------------------------------------
		**  Input:   k    = link index
		**  Output:  *kw  = kwatt energy used
		**           *eff = efficiency (pumps only)
		**  Purpose: computes flow energy associated with link k
		**----------------------------------------------------------------
		*/
	{
		int   i, j;
		double dh, q, e;

		/*** Updated 6/24/02 ***/
		/* No energy if link is closed */
		if (S[k] <= CLOSED)
		{
			*kw = 0.0;
			*eff = 0.0;
			return;
		}
		/*** End of update ***/

		/* Determine flow and head difference */
		q = ABS(Q[k]);
		dh = ABS(H[Link[k].N1] - H[Link[k].N2]);

		/* For pumps, find effic. at current flow */
		if (Link[k].Type == PUMP)
		{
			j = PUMPINDEX(k);
			e = Epump;
			if ((i = Pump[j].Ecurve) > 0)
				e = interp(Curve[i].Npts, Curve[i].X, Curve[i].Y, q * Ucf[FLOW]);
			e = MIN(e, 100.0);
			e = MAX(e, 1.0);
			e /= 100.0;
		}
		else e = 1.0;

		/* Compute energy */
		*kw = dh * q * SpGrav / 8.814 / e * KWperHP;
		*eff = e;
	}


	void  EPANET::tanklevels(long tstep)
		/*
		**----------------------------------------------------------------
		**  Input:   tstep = current time Step
		**  Output:  none
		**  Purpose: computes new water levels in tanks after current
		**           time Step
		**----------------------------------------------------------------
		*/
	{
		int   i, n;
		double dv;

		for (i = 1; i <= Ntanks; i++)
		{

			/* Skip reservoirs */
			if (Tank[i].A == 0.0) continue;

			/* Update the tank's volume & water elevation */
			n = Tank[i].Node;
			dv = D[n] * tstep;
			Tank[i].V += dv;

			/*** Updated 6/24/02 ***/
			/* Check if tank full/empty within next second */
			if (Tank[i].V + D[n] >= Tank[i].Vmax) Tank[i].V = Tank[i].Vmax;
			if (Tank[i].V - D[n] <= Tank[i].Vmin) Tank[i].V = Tank[i].Vmin;

			H[n] = tankgrade(i, Tank[i].V);
		}
	}                       /* End of tanklevels */


	double EPANET::tankvolume(int i, double h)
		/*
		**--------------------------------------------------------------------
		**  Input:   i = tank index
		**           h = water elevation in tank
		**  Output:  returns water volume in tank
		**  Purpose: finds water volume in tank i corresponding to elev. h.
		**--------------------------------------------------------------------
		*/
	{
		int j;

		/* Use level*area if no volume curve */
		j = Tank[i].Vcurve;
		if (j == 0) return(Tank[i].Vmin + (h - Tank[i].Hmin) * Tank[i].A);

		/* If curve exists, interpolate on h to find volume v */
		/* remembering that volume curve is in original units.*/
		else return(interp(Curve[j].Npts, Curve[j].X, Curve[j].Y,
			(h - Node[Tank[i].Node].El) * Ucf[HEAD]) / Ucf[VOLUME]);

	}                       /* End of tankvolume */


	double  EPANET::tankgrade(int i, double v)
		/*
		**-------------------------------------------------------------------
		**  Input:   i = tank index
		**           v = volume in tank
		**  Output:  returns water level in tank
		**  Purpose: finds water level in tank i corresponding to volume v.
		**-------------------------------------------------------------------
		*/
	{
		int j;

		/* Use area if no volume curve */
		j = Tank[i].Vcurve;
		if (j == 0) return(Tank[i].Hmin + (v - Tank[i].Vmin) / Tank[i].A);

		/* If curve exists, interpolate on volume (originally the Y-variable */
		/* but used here as the X-variable) to find new level above bottom.  */
		/* Remember that volume curve is stored in original units.           */
		else return(Node[Tank[i].Node].El +
			interp(Curve[j].Npts, Curve[j].Y, Curve[j].X, v * Ucf[VOLUME]) / Ucf[HEAD]);

	}                        /* End of tankgrade */


	int  EPANET::netsolve(int *iter, double *relerr)
		/*
		**-------------------------------------------------------------------
		**  Input:   none
		**  Output:  *iter   = # of iterations to reach solution
		**           *relerr = convergence error in solution
		**           returns error code
		**  Purpose: solves network nodal equations for heads and flows
		**           using Todini's Gradient algorithm
		**
		*** Updated 9/7/00 ***
		*** Updated 2.00.11 ***
		*** Updated 2.00.12 ***
		**  Notes:   Status checks on CVs, pumps and pipes to tanks are made
		**           every CheckFreq iteration, up until MaxCheck iterations
		**           are reached. Status checks on control valves are made
		**           every iteration if DampLimit = 0 or only when the
		**           convergence error is at or below DampLimit. If DampLimit
		**           is > 0 then future computed flow changes are only 60% of
		**           their full value. A complete status check on all links
		**           is made when convergence is achieved. If convergence is
		**           not achieved in MaxIter trials and ExtraIter > 0 then
		**           another ExtraIter trials are made with no status changes
		**           made to any links and a warning message is generated.
		**
		**   This procedure calls linsolve() which appears in SMATRIX.C.
		**-------------------------------------------------------------------
		*/
	{
		int    i;                     /* Node index */
		int    errcode = 0;           /* Node causing solution error */
		int    nextcheck;             /* Next status check trial */
		int    maxtrials;             /* Max. trials for convergence */
		double newerr;                /* New convergence error */
		int    valveChange;           /* Valve status change flag */
		int    statChange;

		/* Initialize status checking & relaxation factor */
		nextcheck = CheckFreq;
		RelaxFactor = 1.0;

		/* Repeat iterations until convergence or trial limit is exceeded. */
		/* (ExtraIter used to increase trials in case of status cycling.)  */
		if (Statflag == FULL) writerelerr(0, 0);
		maxtrials = MaxIter;
		if (ExtraIter > 0) maxtrials += ExtraIter;
		*iter = 1;
		while (*iter <= maxtrials)
		{
			/*
			** Compute coefficient matrices A & F and solve A*H = F
			** where H = heads, A = Jacobian coeffs. derived from
			** head loss gradients, & F = flow correction terms.
			** Solution for H is returned in F from call to linsolve().
			*/
			newcoeffs();
			errcode = linsolve(Njuncs, Aii, Aij, F);

			/* Take action depending on error code */
			if (errcode < 0) break;    /* Memory allocation problem */
			if (errcode > 0)           /* Ill-conditioning problem */
			{
				/* If control valve causing problem, fix its status & continue, */
				/* otherwise end the iterations with no solution.               */
				if (badvalve(Order[errcode])) continue;
				else break;
			}

			/* Update current solution. */
			/* (Row[i] = row of solution matrix corresponding to node i). */
			for (i = 1; i <= Njuncs; i++) H[i] = F[Row[i]];   /* Update heads */
			newerr = newflows();                          /* Update flows */
			*relerr = newerr;

			/* Write convergence error to status report if called for */
			if (Statflag == FULL) writerelerr(*iter, *relerr);

			/* Apply solution damping & check for change in valve status */
			RelaxFactor = 1.0;
			valveChange = FALSE;
			if (DampLimit > 0.0)
			{
				if (*relerr <= DampLimit)
				{
					RelaxFactor = 0.6;
					valveChange = valvestatus();
				}
			}
			else valveChange = valvestatus();

			/* Check for convergence */
			if (*relerr <= Hacc)
			{
				/* We have convergence. Quit if we are into extra iterations. */
				if (*iter > MaxIter) break;

				/* Quit if no status changes occur. */
				statChange = FALSE;
				if (valveChange)  statChange = TRUE;
				if (linkstatus()) statChange = TRUE;
				if (pswitch())    statChange = TRUE;
				if (!statChange)  break;

				/* We have a status change so continue the iterations */
				nextcheck = *iter + CheckFreq;
			}

			/* No convergence yet. See if its time for a periodic status */
			/* check  on pumps, CV's, and pipes connected to tanks.      */
			else if (*iter <= MaxCheck && *iter == nextcheck)
			{
				linkstatus();
				nextcheck += CheckFreq;
			}
			(*iter)++;
		}

		/* Iterations ended. Report any errors. */
		if (errcode < 0) errcode = 101;      /* Memory allocation error */
		else if (errcode > 0)
		{
			writehyderr(Order[errcode]);      /* Ill-conditioned eqns. error */
			errcode = 110;
		}

		/* Add any emitter flows to junction demands */
		for (i = 1; i <= Njuncs; i++) D[i] += E[i];
		return(errcode);
	}                        /* End of netsolve */


	int  EPANET::badvalve(int n)
		/*
		**-----------------------------------------------------------------
		**  Input:   n = node index
		**  Output:  returns 1 if node n belongs to an active control valve,
		**           0 otherwise
		**  Purpose: determines if a node belongs to an active control valve
		**           whose setting causes an inconsistent set of eqns. If so,
		**           the valve status is fixed open and a warning condition
		**           is generated.
		**-----------------------------------------------------------------
		*/
	{
		int i, k, n1, n2;
		for (i = 1; i <= Nvalves; i++)
		{
			k = Valve[i].Link;
			n1 = Link[k].N1;
			n2 = Link[k].N2;
			if (n == n1 || n == n2)
			{
				if (Link[k].Type == PRV ||
					Link[k].Type == PSV ||
					Link[k].Type == FCV)
				{
					if (S[k] == ACTIVE)
					{
						if (Statflag == FULL)
						{
							sprintf(Msg, FMT61, clocktime(Atime, Htime), Link[k].ID);
							writeline(Msg);
						}
						if (Link[k].Type == FCV) S[k] = XFCV;
						else                     S[k] = XPRESSURE;
						return(1);
					}
				}
				return(0);
			}
		}
		return(0);
	}


	int  EPANET::valvestatus()
		/*
		**-----------------------------------------------------------------
		**  Input:   none
		**  Output:  returns 1 if any pressure or flow control valve                   //(2.00.11 - LR)
		**           changes status, 0 otherwise                                       //(2.00.11 - LR)
		**  Purpose: updates status for PRVs & PSVs whose status                       //(2.00.12 - LR)
		**           is not fixed to OPEN/CLOSED
		**-----------------------------------------------------------------
		*/
	{
		int   change = FALSE,            /* Status change flag      */
			i, k,                       /* Valve & link indexes    */
			n1, n2;                     /* Start & end nodes       */
		char  s;                         /* Valve status settings   */
		double hset;                     /* Valve head setting      */

		for (i = 1; i <= Nvalves; i++)                   /* Examine each valve   */
		{
			k = Valve[i].Link;                        /* Link index of valve  */
			if (K[k] == MISSING) continue;            /* Valve status fixed   */
			n1 = Link[k].N1;                          /* Start & end nodes    */
			n2 = Link[k].N2;
			s = S[k];                                /* Save current status  */

													 //      if (s != CLOSED                           /* No change if flow is */  //(2.00.11 - LR)
													 //      && ABS(Q[k]) < Qtol) continue;            /* negligible.          */  //(2.00.11 - LR)

			switch (Link[k].Type)                     /* Evaluate new status: */
			{
			case PRV:  hset = Node[n2].El + K[k];
				S[k] = prvstatus(k, s, hset, H[n1], H[n2]);
				break;
			case PSV:  hset = Node[n1].El + K[k];
				S[k] = psvstatus(k, s, hset, H[n1], H[n2]);
				break;

				////  FCV status checks moved back into the linkstatus() function ////           //(2.00.12 - LR)
				//         case FCV:  S[k] = fcvstatus(k,s,H[n1],H[n2]);                         //(2.00.12 - LR)
				//                    break;                                                     //(2.00.12 - LR)

			default:   continue;
			}

			/*** Updated 9/7/00 ***/
			/* Do not reset flow in valve if its status changes. */
			/* This strategy improves convergence. */

			/* Check for status change */
			if (s != S[k])
			{
				if (Statflag == FULL) writestatchange(k, s, S[k]);
				change = TRUE;
			}
		}
		return(change);
	}                       /* End of valvestatus() */


							/*** Updated 9/7/00 ***/
	int  EPANET::linkstatus()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns 1 if any link changes status, 0 otherwise
		**  Purpose: determines new status for pumps, CVs, FCVs & pipes                //(2.00.12 - LR)
		**           to tanks.
		**--------------------------------------------------------------
		*/
	{
		int   change = FALSE,             /* Status change flag      */
			k,                          /* Link index              */
			n1,                         /* Start node index        */
			n2;                         /* End node index          */
		double dh;                        /* Head difference         */
		char  status;                     /* Current status          */

										  /* Examine each link */
		for (k = 1; k <= Nlinks; k++)
		{
			n1 = Link[k].N1;
			n2 = Link[k].N2;
			dh = H[n1] - H[n2];

			/* Re-open temporarily closed links (status = XHEAD or TEMPCLOSED) */
			status = S[k];
			if (status == XHEAD || status == TEMPCLOSED) S[k] = OPEN;

			/* Check for status changes in CVs and pumps */
			if (Link[k].Type == CV) S[k] = cvstatus(S[k], dh, Q[k]);
			if (Link[k].Type == PUMP && S[k] >= OPEN && K[k] > 0.0)                  //(2.00.11 - LR)
				S[k] = pumpstatus(k, -dh);

			/* Check for status changes in non-fixed FCVs */
			if (Link[k].Type == FCV && K[k] != MISSING)                              //(2.00.12 - LR)//
				S[k] = fcvstatus(k, status, H[n1], H[n2]);                               //(2.00.12 - LR)//

																						 /* Check for flow into (out of) full (empty) tanks */
			if (n1 > Njuncs || n2 > Njuncs) tankstatus(k, n1, n2);

			/* Note change in link status; do not revise link flow */                //(2.00.11 - LR)
			if (status != S[k])
			{
				change = TRUE;
				if (Statflag == FULL) writestatchange(k, status, S[k]);

				//if (S[k] <= CLOSED) Q[k] = QZERO;                                   //(2.00.11 - LR)
				//else setlinkflow(k, dh);                                            //(2.00.11 - LR)
			}
		}
		return(change);
	}                        /* End of linkstatus */


	char  EPANET::cvstatus(char s, double dh, double q)
		/*
		**--------------------------------------------------
		**  Input:   s  = current status
		**           dh = headloss
		**           q  = flow
		**  Output:  returns new link status
		**  Purpose: updates status of a check valve.
		**--------------------------------------------------
		*/
	{
		/* Prevent reverse flow through CVs */
		if (ABS(dh) > Htol)
		{
			if (dh < -Htol)     return(CLOSED);
			else if (q < -Qtol) return(CLOSED);
			else                return(OPEN);
		}
		else
		{
			if (q < -Qtol) return(CLOSED);
			else           return(s);
		}
	}


	char  EPANET::pumpstatus(int k, double dh)
		/*
		**--------------------------------------------------
		**  Input:   k  = link index
		**           dh = head gain
		**  Output:  returns new pump status
		**  Purpose: updates status of an open pump.
		**--------------------------------------------------
		*/
	{
		int   p;
		double hmax;

		/* Prevent reverse flow through pump */
		p = PUMPINDEX(k);
		if (Pump[p].Ptype == CONST_HP) hmax = BIG;
		else hmax = SQR(K[k]) * Pump[p].Hmax;
		if (dh > hmax + Htol) return(XHEAD);

		/*** Flow higher than pump curve no longer results in a status change ***/     //(2.00.11 - LR)
																					   /* Check if pump cannot deliver flow */                                     //(2.00.11 - LR)
																																								   //if (Q[k] > K[k]*Pump[p].Qmax + Qtol) return(XFLOW);                       //(2.00.11 - LR)
		return(OPEN);
	}


	char EPANET::prvstatus(int k, char s, double hset, double h1, double h2)
		/*
		**-----------------------------------------------------------
		**  Input:   k    = link index
		**           s    = current status
		**           hset = valve head setting
		**           h1   = head at upstream node
		**           h2   = head at downstream node
		**  Output:  returns new valve status
		**  Purpose: updates status of a pressure reducing valve.
		**-----------------------------------------------------------
		*/
	{
		char  status;     /* New valve status */
		double hml;        /* Minor headloss   */
		double htol = Htol;

		status = s;
		if (K[k] == MISSING) return(status);       /* Status fixed by user */
		hml = Link[k].Km * SQR(Q[k]);                /* Head loss when open  */

												   /*** Status rules below have changed. ***/                                     //(2.00.11 - LR)

		switch (s)
		{
		case ACTIVE:
			if (Q[k] < -Qtol)            status = CLOSED;
			else if (h1 - hml < hset - htol) status = OPEN;                           //(2.00.11 - LR)
			else                         status = ACTIVE;
			break;
		case OPEN:
			if (Q[k] < -Qtol)            status = CLOSED;
			else if (h2 >= hset + htol)    status = ACTIVE;                         //(2.00.11 - LR)
			else                         status = OPEN;
			break;
		case CLOSED:
			if (h1 >= hset + htol                                                  //(2.00.11 - LR)
				&& h2 < hset - htol)         status = ACTIVE;                         //(2.00.11 - LR)
			else if (h1 < hset - htol                                               //(2.00.11 - LR)
				&& h1 > h2 + htol)       status = OPEN;                           //(2.00.11 - LR)
			else                         status = CLOSED;
			break;
		case XPRESSURE:
			if (Q[k] < -Qtol)            status = CLOSED;
			break;
		}
		return(status);
	}


	char  EPANET::psvstatus(int k, char s, double hset, double h1, double h2)
		/*
		**-----------------------------------------------------------
		**  Input:   k    = link index
		**           s    = current status
		**           hset = valve head setting
		**           h1   = head at upstream node
		**           h2   = head at downstream node
		**  Output:  returns new valve status
		**  Purpose: updates status of a pressure sustaining valve.
		**-----------------------------------------------------------
		*/
	{
		char  status;       /* New valve status */
		double hml;          /* Minor headloss   */
		double htol = Htol;

		status = s;
		if (K[k] == MISSING) return(status);       /* Status fixed by user */
		hml = Link[k].Km * SQR(Q[k]);                /* Head loss when open  */

												   /*** Status rules below have changed. ***/                                     //(2.00.11 - LR)

		switch (s)
		{
		case ACTIVE:
			if (Q[k] < -Qtol)            status = CLOSED;
			else if (h2 + hml > hset + htol) status = OPEN;                           //(2.00.11 - LR)
			else                         status = ACTIVE;
			break;
		case OPEN:
			if (Q[k] < -Qtol)            status = CLOSED;
			else if (h1 < hset - htol)     status = ACTIVE;                         //(2.00.11 - LR)
			else                         status = OPEN;
			break;
		case CLOSED:
			if (h2 > hset + htol                                                    //(2.00.11 - LR)
				&& h1 > h2 + htol)            status = OPEN;                           //(2.00.11 - LR)
			else if (h1 >= hset + htol                                              //(2.00.11 - LR)
				&& h1 > h2 + htol)       status = ACTIVE;                         //(2.00.11 - LR)
			else                         status = CLOSED;
			break;
		case XPRESSURE:
			if (Q[k] < -Qtol)            status = CLOSED;
			break;
		}
		return(status);
	}


	char  EPANET::fcvstatus(int k, char s, double h1, double h2)
		/*
		**-----------------------------------------------------------
		**  Input:   k    = link index
		**           s    = current status
		**           h1   = head at upstream node
		**           h2   = head at downstream node
		**  Output:  returns new valve status
		**  Purpose: updates status of a flow control valve.
		**
		**    Valve status changes to XFCV if flow reversal.
		**    If current status is XFCV and current flow is
		**    above setting, then valve becomes active.
		**    If current status is XFCV, and current flow
		**    positive but still below valve setting, then
		**    status remains same.
		**-----------------------------------------------------------
		*/
	{
		char  status;        /* New valve status */
		status = s;
		if (h1 - h2 < -Htol)                status = XFCV;
		else if (Q[k] < -Qtol)            status = XFCV;                          //(2.00.11 - LR)
		else if (s == XFCV && Q[k] >= K[k]) status = ACTIVE;
		return(status);
	}


	/*** Updated 9/7/00 ***/
	/*** Updated 11/19/01 ***/
	void  EPANET::tankstatus(int k, int n1, int n2)
		/*
		**----------------------------------------------------------------
		**  Input:   k  = link index
		**           n1 = start node of link
		**           n2 = end node of link
		**  Output:  none
		**  Purpose: closes link flowing into full or out of empty tank
		**----------------------------------------------------------------
		*/
	{
		int   i, n;
		double h, q;

		/* Make node n1 be the tank */
		q = Q[k];
		i = n1 - Njuncs;
		if (i <= 0)
		{
			i = n2 - Njuncs;
			if (i <= 0) return;
			n = n1;
			n1 = n2;
			n2 = n;
			q = -q;
		}
		h = H[n1] - H[n2];

		/* Skip reservoirs & closed links */
		if (Tank[i].A == 0.0 || S[k] <= CLOSED) return;

		/* If tank full, then prevent flow into it */
		if (H[n1] >= Tank[i].Hmax - Htol)
		{

			/* Case 1: Link is a pump discharging into tank */
			if (Link[k].Type == PUMP)
			{
				if (Link[k].N2 == n1) S[k] = TEMPCLOSED;
			}

			/* Case 2: Downstream head > tank head */
			/* (i.e., an open outflow check valve would close) */
			else if (cvstatus(OPEN, h, q) == CLOSED) S[k] = TEMPCLOSED;
		}

		/* If tank empty, then prevent flow out of it */
		if (H[n1] <= Tank[i].Hmin + Htol)
		{

			/* Case 1: Link is a pump discharging from tank */
			if (Link[k].Type == PUMP)
			{
				if (Link[k].N1 == n1) S[k] = TEMPCLOSED;
			}

			/* Case 2: Tank head > downstream head */
			/* (i.e., a closed outflow check valve would open) */
			else if (cvstatus(CLOSED, h, q) == OPEN) S[k] = TEMPCLOSED;
		}
	}                        /* End of tankstatus */


	int  EPANET::pswitch()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns 1 if status of any link changes, 0 if not
		**  Purpose: adjusts settings of links controlled by junction
		**           pressures after a hydraulic solution is found
		**--------------------------------------------------------------
		*/
	{
		int   i,                 /* Control statement index */
			k,                 /* Link being controlled */
			n,                 /* Node controlling link */
			reset,             /* Flag on control conditions */
			change,            /* Flag for status or setting change */
			anychange = 0;     /* Flag for 1 or more changes */
		char  s;                 /* Current link status */

								 /* Check each control statement */
		for (i = 1; i <= Ncontrols; i++)
		{
			reset = 0;
			if ((k = Control[i].Link) <= 0) continue;

			/* Determine if control based on a junction, not a tank */
			if ((n = Control[i].Node) > 0 && n <= Njuncs)
			{
				/* Determine if control conditions are satisfied */
				if (Control[i].Type == LOWLEVEL
					&& H[n] <= Control[i].Grade + Htol)
					reset = 1;
				if (Control[i].Type == HILEVEL
					&& H[n] >= Control[i].Grade - Htol)
					reset = 1;
			}

			/* Determine if control forces a status or setting change */
			if (reset == 1)
			{
				change = 0;
				s = S[k];
				if (Link[k].Type == PIPE)
				{
					if (s != Control[i].Status) change = 1;
				}
				if (Link[k].Type == PUMP)
				{
					if (K[k] != Control[i].Setting) change = 1;
				}
				if (Link[k].Type >= PRV)
				{
					if (K[k] != Control[i].Setting) change = 1;
					else if (K[k] == MISSING &&
						s != Control[i].Status) change = 1;
				}

				/* If a change occurs, update status & setting */
				if (change)
				{
					S[k] = Control[i].Status;
					if (Link[k].Type > PIPE) K[k] = Control[i].Setting;
					if (Statflag == FULL) writestatchange(k, s, S[k]);

					/* Re-set flow if status has changed */
					//            if (S[k] != s) initlinkflow(k, S[k], K[k]);
					anychange = 1;
				}
			}
		}
		return(anychange);
	}                        /* End of pswitch */


	double EPANET::newflows()
		/*
		**----------------------------------------------------------------
		**  Input:   none
		**  Output:  returns solution convergence error
		**  Purpose: updates link flows after new nodal heads computed
		**----------------------------------------------------------------
		*/
	{
		double  dh,                    /* Link head loss       */
			dq;                    /* Link flow change     */
		double  dqsum,                 /* Network flow change  */
			qsum;                  /* Network total flow   */
		int   k, n, n1, n2;

		/* Initialize net inflows (i.e., demands) at tanks */
		for (n = Njuncs + 1; n <= Nnodes; n++) D[n] = 0.0;

		/* Initialize sum of flows & corrections */
		qsum = 0.0;
		dqsum = 0.0;

		/* Update flows in all links */
		for (k = 1; k <= Nlinks; k++)
		{

			/*
			** Apply flow update formula:
			**   dq = Y - P*(new head loss)
			**    P = 1/(dh/dq)
			**    Y = P*(head loss based on current flow)
			** where P & Y were computed in newcoeffs().
			*/

			n1 = Link[k].N1;
			n2 = Link[k].N2;
			dh = H[n1] - H[n2];
			dq = Y[k] - P[k] * dh;

			/* Adjust flow change by the relaxation factor */                        //(2.00.11 - LR)
			dq *= RelaxFactor;                                                       //(2.00.11 - LR)

																					 /* Prevent flow in constant HP pumps from going negative */
			if (Link[k].Type == PUMP)
			{
				n = PUMPINDEX(k);
				if (Pump[n].Ptype == CONST_HP && dq > Q[k]) dq = Q[k] / 2.0;
			}
			Q[k] -= dq;

			/* Update sum of absolute flows & flow corrections */
			qsum += ABS(Q[k]);
			dqsum += ABS(dq);

			/* Update net flows to tanks */
			if (S[k] > CLOSED)                                                     //(2.00.12 - LR)
			{
				if (n1 > Njuncs) D[n1] -= Q[k];
				if (n2 > Njuncs) D[n2] += Q[k];
			}

		}

		/* Update emitter flows */
		for (k = 1; k <= Njuncs; k++)
		{
			if (Node[k].Ke == 0.0) continue;
			dq = emitflowchange(k);
			E[k] -= dq;
			qsum += ABS(E[k]);
			dqsum += ABS(dq);
		}

		/* Return ratio of total flow corrections to total flow */
		if (qsum > Hacc) return(dqsum / qsum);
		else return(dqsum);

	}                        /* End of newflows */


	void   EPANET::newcoeffs()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: computes coefficients of linearized network eqns.
		**--------------------------------------------------------------
		*/
	{
		memset(Aii, 0, (Nnodes + 1) * sizeof(double));   /* Reset coeffs. to 0 */
		memset(Aij, 0, (Ncoeffs + 1) * sizeof(double));
		memset(F, 0, (Nnodes + 1) * sizeof(double));
		memset(X, 0, (Nnodes + 1) * sizeof(double));
		memset(P, 0, (Nlinks + 1) * sizeof(double));
		memset(Y, 0, (Nlinks + 1) * sizeof(double));
		linkcoeffs();                            /* Compute link coeffs.  */
		emittercoeffs();                         /* Compute emitter coeffs.*/
		nodecoeffs();                            /* Compute node coeffs.  */
		valvecoeffs();                           /* Compute valve coeffs. */
	}                        /* End of newcoeffs */


	void  EPANET::linkcoeffs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: computes solution matrix coefficients for links
		**--------------------------------------------------------------
		*/
	{
		int   k, n1, n2;

		/* Examine each link of network */
		for (k = 1; k <= Nlinks; k++)
		{
			n1 = Link[k].N1;           /* Start node of link */
			n2 = Link[k].N2;           /* End node of link   */

									   /* Compute P[k] = 1 / (dh/dQ) and Y[k] = h * P[k]   */
									   /* for each link k (where h = link head loss).      */
									   /* FCVs, PRVs, and PSVs with non-fixed status       */
									   /* are analyzed later.                              */

			switch (Link[k].Type)
			{
			case CV:
			case PIPE:  pipecoeff(k); break;
			case PUMP:  pumpcoeff(k); break;
			case PBV:   pbvcoeff(k);  break;
			case TCV:   tcvcoeff(k);  break;
			case GPV:   gpvcoeff(k);  break;
			case FCV:
			case PRV:
			case PSV:   /* If valve status fixed then treat as pipe */
						/* otherwise ignore the valve for now. */
				if (K[k] == MISSING) valvecoeff(k);  //pipecoeff(k);      //(2.00.11 - LR)    
				else continue;
				break;
			default:    continue;
			}

			/* Update net nodal inflows (X), solution matrix (A) and RHS array (F) */
			/* (Use covention that flow out of node is (-), flow into node is (+)) */
			X[n1] -= Q[k];
			X[n2] += Q[k];
			Aij[Ndx[k]] -= P[k];              /* Off-diagonal coeff. */
			if (n1 <= Njuncs)                 /* Node n1 is junction */
			{
				Aii[Row[n1]] += P[k];          /* Diagonal coeff. */
				F[Row[n1]] += Y[k];            /* RHS coeff.      */
			}
			else F[Row[n2]] += (P[k] * H[n1]);  /* Node n1 is a tank   */
			if (n2 <= Njuncs)                 /* Node n2 is junction */
			{
				Aii[Row[n2]] += P[k];          /* Diagonal coeff. */
				F[Row[n2]] -= Y[k];            /* RHS coeff.      */
			}
			else  F[Row[n1]] += (P[k] * H[n2]); /* Node n2 is a tank   */
		}
	}                        /* End of linkcoeffs */


	void  EPANET::nodecoeffs()
		/*
		**----------------------------------------------------------------
		**  Input:   none
		**  Output:  none
		**  Purpose: completes calculation of nodal flow imbalance (X)
		**           & flow correction (F) arrays
		**----------------------------------------------------------------
		*/
	{
		int   i;

		/* For junction nodes, subtract demand flow from net */
		/* flow imbalance & add imbalance to RHS array F.    */
		for (i = 1; i <= Njuncs; i++)
		{
			X[i] -= D[i];
			F[Row[i]] += X[i];
		}
	}                        /* End of nodecoeffs */


	void  EPANET::valvecoeffs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: computes matrix coeffs. for PRVs, PSVs & FCVs
		**            whose status is not fixed to OPEN/CLOSED
		**--------------------------------------------------------------
		*/
	{
		int i, k, n1, n2;

		for (i = 1; i <= Nvalves; i++)                   /* Examine each valve   */
		{
			k = Valve[i].Link;                        /* Link index of valve  */
			if (K[k] == MISSING) continue;            /* Valve status fixed   */
			n1 = Link[k].N1;                          /* Start & end nodes    */
			n2 = Link[k].N2;
			switch (Link[k].Type)                     /* Call valve-specific  */
			{                                         /*   function           */
			case PRV:  prvcoeff(k, n1, n2); break;
			case PSV:  psvcoeff(k, n1, n2); break;
			case FCV:  fcvcoeff(k, n1, n2); break;
			default:   continue;
			}
		}
	}                        /* End of valvecoeffs */


	void EPANET::emittercoeffs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: computes matrix coeffs. for emitters
		**
		**   Note: Emitters consist of a fictitious pipe connected to
		**         a fictitious reservoir whose elevation equals that
		**         of the junction. The headloss through this pipe is
		**         Ke*(Flow)^Qexp, where Ke = emitter headloss coeff.
		**--------------------------------------------------------------
		*/
	{
		int   i;
		double  ke;
		double  p;
		double  q;
		double  y;
		double  z;
		for (i = 1; i <= Njuncs; i++)
		{
			if (Node[i].Ke == 0.0) continue;
			ke = MAX(CSMALL, Node[i].Ke);
			q = E[i];
			z = ke * pow(ABS(q), Qexp);
			p = Qexp * z / ABS(q);
			if (p < RQtol) p = 1.0 / RQtol;
			else p = 1.0 / p;
			y = SGN(q) * z * p;
			Aii[Row[i]] += p;
			F[Row[i]] += y + p * Node[i].El;
			X[i] -= q;
		}
	}


	double EPANET::emitflowchange(int i)
		/*
		**--------------------------------------------------------------
		**   Input:   i = node index
		**   Output:  returns change in flow at an emitter node
		**   Purpose: computes flow change at an emitter node
		**--------------------------------------------------------------
		*/
	{
		double ke, p;
		ke = MAX(CSMALL, Node[i].Ke);
		p = Qexp * ke * pow(ABS(E[i]), (Qexp - 1.0));
		if (p < RQtol)
			p = 1 / RQtol;
		else
			p = 1.0 / p;
		return(E[i] / Qexp - p * (H[i] - Node[i].El));
	}


	void EPANET::pipecoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**  Purpose:  computes P & Y coefficients for pipe k
		**
		**    P = inverse head loss gradient = 1/(dh/dQ)
		**    Y = flow correction term = h*P
		**--------------------------------------------------------------
		*/
	{
		double  hpipe,     /* Normal head loss          */
			hml,       /* Minor head loss           */
			ml,        /* Minor loss coeff.         */
			p,         /* q*(dh/dq)                 */
			q,         /* Abs. value of flow        */
			r,         /* Resistance coeff.         */
			r1,        /* Total resistance factor   */
			f,         /* D-W friction factor       */
			dfdq;      /* Derivative of fric. fact. */

					   /* For closed pipe use headloss formula: h = CBIG*q */
		if (S[k] <= CLOSED)
		{
			P[k] = 1.0 / CBIG;
			Y[k] = Q[k];
			return;
		}

		/* Evaluate headloss coefficients */
		q = ABS(Q[k]);                         /* Absolute flow       */
		ml = Link[k].Km;                       /* Minor loss coeff.   */
		r = Link[k].R;                         /* Resistance coeff.   */
		f = 1.0;                               /* D-W friction factor */
		if (Formflag == DW) f = DWcoeff(k, &dfdq);
		r1 = f * r + ml;

		/* Use large P coefficient for small flow resistance product */
		if (r1 * q < RQtol)
		{
			P[k] = 1.0 / RQtol;
			Y[k] = Q[k] / Hexp;
			return;
		}

		/* Compute P and Y coefficients */
		if (Formflag == DW)                  /* D-W eqn. */
		{
			hpipe = r1 * SQR(q);                /* Total head loss */
			p = 2.0 * r1 * q;                     /* |dh/dQ| */
											  /* + dfdq*r*q*q;*/                 /* Ignore df/dQ term */
			p = 1.0 / p;
			P[k] = p;
			Y[k] = SGN(Q[k]) * hpipe * p;
		}
		else                                 /* H-W or C-M eqn.   */
		{
			hpipe = r * pow(q, Hexp);            /* Friction head loss  */
			p = Hexp * hpipe;                   /* Q*dh(friction)/dQ   */
			if (ml > 0.0)
			{
				hml = ml * q * q;                  /* Minor head loss   */
				p += 2.0 * hml;                  /* Q*dh(Total)/dQ    */
			}
			else  hml = 0.0;
			p = Q[k] / p;                       /* 1 / (dh/dQ) */
			P[k] = ABS(p);
			Y[k] = p * (hpipe + hml);
		}
	}                        /* End of pipecoeff */


	double EPANET::DWcoeff(int k, double *dfdq)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  returns Darcy-Weisbach friction factor
		**   Purpose: computes Darcy-Weisbach friction factor
		**
		**    Uses interpolating polynomials developed by
		**    E. Dunlop for transition flow from 2000 < Re < 4000.
		**
		**   df/dq term is ignored as it slows convergence rate.
		**--------------------------------------------------------------
		*/
	{
		double q,             /* Abs. value of flow */
			f;             /* Friction factor    */
		double x1, x2, x3, x4,
			y1, y2, y3,
			fa, fb, r;
		double s, w;

		*dfdq = 0.0;
		if (Link[k].Type > PIPE) return(1.0); /* Only apply to pipes */
		q = ABS(Q[k]);
		s = Viscos * Link[k].Diam;
		w = q / s;                       /* w = Re(Pi/4) */
		if (w >= A1)                   /* Re >= 4000; Colebrook Formula */
		{
			y1 = A8 / pow(w, 0.9);
			y2 = Link[k].Kc / (3.7 * Link[k].Diam) + y1;
			y3 = A9 * log(y2);
			f = 1.0 / SQR(y3);
			/*  *dfdq = (2.0+AA*y1/(y2*y3))*f; */   /* df/dq */
		}
		else if (w > A2)              /* Re > 2000; Interpolation formula */
		{
			y2 = Link[k].Kc / (3.7 * Link[k].Diam) + AB;
			y3 = A9 * log(y2);
			fa = 1.0 / SQR(y3);
			fb = (2.0 + AC / (y2 * y3)) * fa;
			r = w / A2;
			x1 = 7.0 * fa - fb;
			x2 = 0.128 - 17.0 * fa + 2.5 * fb;
			x3 = -0.128 + 13.0 * fa - (fb + fb);
			x4 = r * (0.032 - 3.0 * fa + 0.5 * fb);
			f = x1 + r * (x2 + r * (x3 + x4));
			/*  *dfdq = (x1 + x1 + r*(3.0*x2 + r*(4.0*x3 + 5.0*x4)));  */
		}
		else if (w > A4)              /* Laminar flow: Hagen-Poiseuille Formula */
		{
			f = A3 * s / q;
			/*  *dfdq = A3*s; */
		}
		else
		{
			f = 8.0;
			*dfdq = 0.0;
		}
		return(f);
	}                        /* End of DWcoeff */


							 /*** Updated 10/25/00 ***/
							 /*** Updated 12/29/00 ***/
	void EPANET::pumpcoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**   Purpose: computes P & Y coeffs. for pump in link k
		**--------------------------------------------------------------
		*/
	{
		int   p;         /* Pump index             */
		double h0,       /* Shutoff head           */
			q,         /* Abs. value of flow     */
			r,         /* Flow resistance coeff. */
			n;         /* Flow exponent coeff.   */

					   /* Use high resistance pipe if pump closed or cannot deliver head */
		if (S[k] <= CLOSED || K[k] == 0.0)
		{
			//pipecoeff(k);                                                          //(2.00.11 - LR)
			P[k] = 1.0 / CBIG;                                                         //(2.00.11 - LR)
			Y[k] = Q[k];                                                             //(2.00.11 - LR)
			return;
		}

		q = ABS(Q[k]);
		q = MAX(q, TINY);
		p = PUMPINDEX(k);

		/* Get pump curve coefficients for custom pump curve. */
		if (Pump[p].Ptype == CUSTOM)
		{
			/* Find intercept (h0) & slope (r) of pump curve    */
			/* line segment which contains speed-adjusted flow. */
			curvecoeff(Pump[p].Hcurve, q / K[k], &h0, &r);

			/* Determine head loss coefficients. */
			Pump[p].H0 = -h0;
			Pump[p].R = -r;
			Pump[p].N = 1.0;
		}

		/* Adjust head loss coefficients for pump speed. */
		h0 = SQR(K[k]) * Pump[p].H0;
		n = Pump[p].N;
		r = Pump[p].R * pow(K[k], 2.0 - n);
		if (n != 1.0) r = n * r * pow(q, n - 1.0);

		/* Compute inverse headloss gradient (P) and flow correction factor (Y) */
		P[k] = 1.0 / MAX(r, RQtol);
		Y[k] = Q[k] / n + P[k] * h0;
	}                        /* End of pumpcoeff */


							 /*** Updated 10/25/00 ***/
							 /*** Updated 12/29/00 ***/
	void  EPANET::curvecoeff(int i, double q, double *h0, double *r)
		/*
		**-------------------------------------------------------------------
		**   Input:   i   = curve index
		**            q   = flow rate
		**   Output:  *h0  = head at zero flow (y-intercept)
		**            *r  = dHead/dFlow (slope)
		**   Purpose: computes intercept and slope of head v. flow curve
		**            at current flow.
		**-------------------------------------------------------------------
		*/
	{
		int   k1, k2, npts;
		double *x, *y;

		/* Remember that curve is stored in untransformed units */
		q *= Ucf[FLOW];
		x = Curve[i].X;           /* x = flow */
		y = Curve[i].Y;           /* y = head */
		npts = Curve[i].Npts;

		/* Find linear segment of curve that brackets flow q */
		k2 = 0;
		while (k2 < npts && x[k2] < q) k2++;
		if (k2 == 0)    k2++;
		else if (k2 == npts) k2--;
		k1 = k2 - 1;

		/* Compute slope and intercept of this segment */
		*r = (y[k2] - y[k1]) / (x[k2] - x[k1]);
		*h0 = y[k1] - (*r) * x[k1];

		/* Convert units */
		*h0 = (*h0) / Ucf[HEAD];
		*r = (*r) * Ucf[FLOW] / Ucf[HEAD];
	}                       /* End of curvecoeff */


	void  EPANET::gpvcoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**   Purpose: computes P & Y coeffs. for general purpose valve
		**--------------------------------------------------------------
		*/
	{
		double h0,        /* Headloss curve intercept */
			q,         /* Abs. value of flow       */
			r;         /* Flow resistance coeff.   */

					   /*** Updated 9/7/00 ***/
					   /* Treat as a pipe if valve closed */
		if (S[k] == CLOSED) valvecoeff(k); //pipecoeff(k);                          //(2.00.11 - LR)

										   /* Otherwise utilize headloss curve   */
										   /* whose index is stored in K */
		else
		{
			/* Find slope & intercept of headloss curve. */
			q = ABS(Q[k]);
			q = MAX(q, TINY);

			/*** Updated 10/25/00 ***/
			/*** Updated 12/29/00 ***/
			curvecoeff((int)ROUND(K[k]), q, &h0, &r);

			/* Compute inverse headloss gradient (P) */
			/* and flow correction factor (Y).       */
			P[k] = 1.0 / MAX(r, RQtol);
			Y[k] = P[k] * (h0 + r * q) * SGN(Q[k]);                                        //(2.00.11 - LR)
		}
	}


	void  EPANET::pbvcoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**   Purpose: computes P & Y coeffs. for pressure breaker valve
		**--------------------------------------------------------------
		*/
	{
		/* If valve fixed OPEN or CLOSED then treat as a pipe */
		if (K[k] == MISSING || K[k] == 0.0) valvecoeff(k);  //pipecoeff(k);         //(2.00.11 - LR)

															/* If valve is active */
		else
		{
			/* Treat as a pipe if minor loss > valve setting */
			if (Link[k].Km * SQR(Q[k]) > K[k]) valvecoeff(k);  //pipecoeff(k);         //(2.00.11 - LR)

															 /* Otherwise force headloss across valve to be equal to setting */
			else
			{
				P[k] = CBIG;
				Y[k] = K[k] * CBIG;
			}
		}
	}                        /* End of pbvcoeff */


	void  EPANET::tcvcoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**   Purpose: computes P & Y coeffs. for throttle control valve
		**--------------------------------------------------------------
		*/
	{
		double km;

		/* Save original loss coeff. for open valve */
		km = Link[k].Km;

		/* If valve not fixed OPEN or CLOSED, compute its loss coeff. */
		if (K[k] != MISSING)
			Link[k].Km = 0.02517 * K[k] / (SQR(Link[k].Diam) * SQR(Link[k].Diam));

		/* Then apply usual pipe formulas */
		valvecoeff(k);  //pipecoeff(k);                                             //(2.00.11 - LR)

						/* Restore original loss coeff. */
		Link[k].Km = km;
	}                        /* End of tcvcoeff */


	void  EPANET::prvcoeff(int k, int n1, int n2)
		/*
		**--------------------------------------------------------------
		**   Input:   k    = link index
		**            n1   = upstream node of valve
		**            n2   = downstream node of valve
		**   Output:  none
		**   Purpose: computes solution matrix coeffs. for pressure
		**            reducing valves
		**--------------------------------------------------------------
		*/
	{
		int   i, j;                       /* Rows of solution matrix */
		double hset;                      /* Valve head setting      */
		i = Row[n1];                    /* Matrix rows of nodes    */
		j = Row[n2];
		hset = Node[n2].El + K[k];     /* Valve setting           */

		if (S[k] == ACTIVE)
		{
			/*
			Set coeffs. to force head at downstream
			node equal to valve setting & force flow (when updated in
			newflows()) equal to flow imbalance at downstream node.
			*/
			P[k] = 0.0;
			Y[k] = Q[k] + X[n2];       /* Force flow balance   */
			F[j] += (hset * CBIG);       /* Force head = hset    */
			Aii[j] += CBIG;            /*   at downstream node */
			if (X[n2] < 0.0) F[i] += X[n2];
			return;
		}

		/*
		For OPEN, CLOSED, or XPRESSURE valve
		compute matrix coeffs. using the valvecoeff() function.                  //(2.00.11 - LR)
		*/
		valvecoeff(k);  /*pipecoeff(k);*/                                           //(2.00.11 - LR)
		Aij[Ndx[k]] -= P[k];
		Aii[i] += P[k];
		Aii[j] += P[k];
		F[i] += (Y[k] - Q[k]);
		F[j] -= (Y[k] - Q[k]);
	}                        /* End of prvcoeff */


	void  EPANET::psvcoeff(int k, int n1, int n2)
		/*
		**--------------------------------------------------------------
		**   Input:   k    = link index
		**            n1   = upstream node of valve
		**            n2   = downstream node of valve
		**   Output:  none
		**   Purpose: computes solution matrix coeffs. for pressure
		**            sustaining valve
		**--------------------------------------------------------------
		*/
	{
		int   i, j;                       /* Rows of solution matrix */
		double hset;                      /* Valve head setting      */
		i = Row[n1];                    /* Matrix rows of nodes    */
		j = Row[n2];
		hset = Node[n1].El + K[k];     /* Valve setting           */

		if (S[k] == ACTIVE)
		{
			/*
			Set coeffs. to force head at upstream
			node equal to valve setting & force flow (when updated in
			newflows()) equal to flow imbalance at upstream node.
			*/
			P[k] = 0.0;
			Y[k] = Q[k] - X[n1];              /* Force flow balance   */
			F[i] += (hset * CBIG);              /* Force head = hset    */
			Aii[i] += CBIG;                   /*   at upstream node   */
			if (X[n1] > 0.0) F[j] += X[n1];
			return;
		}

		/*
		For OPEN, CLOSED, or XPRESSURE valve
		compute matrix coeffs. using the valvecoeff() function.                  //(2.00.11 - LR)
		*/
		valvecoeff(k);  /*pipecoeff(k);*/                                           //(2.00.11 - LR)
		Aij[Ndx[k]] -= P[k];
		Aii[i] += P[k];
		Aii[j] += P[k];
		F[i] += (Y[k] - Q[k]);
		F[j] -= (Y[k] - Q[k]);
	}                        /* End of psvcoeff */


	void  EPANET::fcvcoeff(int k, int n1, int n2)
		/*
		**--------------------------------------------------------------
		**   Input:   k    = link index
		**            n1   = upstream node of valve
		**            n2   = downstream node of valve
		**   Output:  none
		**   Purpose: computes solution matrix coeffs. for flow control
		**            valve
		**--------------------------------------------------------------
		*/
	{
		int   i, j;                   /* Rows in solution matrix */
		double q;                     /* Valve flow setting      */
		q = K[k];
		i = Row[n1];
		j = Row[n2];

		/*
		If valve active, break network at valve and treat
		flow setting as external demand at upstream node
		and external supply at downstream node.
		*/
		if (S[k] == ACTIVE)
		{
			X[n1] -= q;
			F[i] -= q;
			X[n2] += q;
			F[j] += q;
			/*P[k] = 0.0;*/
			P[k] = 1.0 / CBIG;                                                         //(2.00.11 - LR)
			Aij[Ndx[k]] -= P[k];                                                     //(2.00.11 - LR)
			Aii[i] += P[k];                                                          //(2.00.11 - LR)
			Aii[j] += P[k];                                                          //(2.00.11 - LR)
			Y[k] = Q[k] - q;
		}
		/*
		Otherwise treat valve as an open pipe
		*/
		else
		{
			valvecoeff(k);  //pipecoeff(k);                                          //(2.00.11 - LR)
			Aij[Ndx[k]] -= P[k];
			Aii[i] += P[k];
			Aii[j] += P[k];
			F[i] += (Y[k] - Q[k]);
			F[j] -= (Y[k] - Q[k]);
		}
	}                        /* End of fcvcoeff */


							 /*** New function added. ***/                                                  //(2.00.11 - LR)
	void EPANET::valvecoeff(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k    = link index
		**   Output:  none
		**   Purpose: computes solution matrix coeffs. for a completely
		**            open, closed, or throttled control valve.
		**--------------------------------------------------------------
		*/
	{
		double p;

		// Valve is closed. Use a very small matrix coeff.
		if (S[k] <= CLOSED)
		{
			P[k] = 1.0 / CBIG;
			Y[k] = Q[k];
			return;
		}

		// Account for any minor headloss through the valve
		if (Link[k].Km > 0.0)
		{
			p = 2.0 * Link[k].Km * fabs(Q[k]);
			if (p < RQtol) p = RQtol;
			P[k] = 1.0 / p;
			Y[k] = Q[k] / 2.0;
		}
		else
		{
			P[k] = 1.0 / RQtol;
			Y[k] = Q[k];
		}
	}

	/*smatrix*/
	int  EPANET::createsparse()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  returns error code
		** Purpose: creates sparse representation of coeff. matrix
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;

		/* Allocate data structures */
		ERRCODE(allocsparse());
		if (errcode) return(errcode);

		/* Build node-link adjacency lists with parallel links removed. */
		Degree = (int *)calloc(Nnodes + 1, sizeof(int));
		ERRCODE(MEMCHECK(Degree));
		ERRCODE(buildlists(TRUE));
		if (!errcode)
		{
			xparalinks();    /* Remove parallel links */
			countdegree();   /* Find degree of each junction */
		}                   /* (= # of adjacent links)  */

							/* Re-order nodes to minimize number of non-zero coeffs.    */
							/* in factorized solution matrix. At same time, adjacency   */
							/* list is updated with links representing non-zero coeffs. */
		Ncoeffs = Nlinks;
		ERRCODE(reordernodes());

		/* Allocate memory for sparse storage of positions of non-zero */
		/* coeffs. and store these positions in vector NZSUB. */
		ERRCODE(storesparse(Njuncs));

		/* Free memory used for adjacency lists and sort */
		/* row indexes in NZSUB to optimize linsolve().  */
		if (!errcode) freelists();
		ERRCODE(ordersparse(Njuncs));

		/* Re-build adjacency lists without removing parallel */
		/* links for use in future connectivity checking.     */
		ERRCODE(buildlists(FALSE));

		/* Free allocated memory */
		free(Degree);
		return(errcode);
	}                        /* End of createsparse */


	int  EPANET::allocsparse()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  returns error code
		** Purpose: allocates memory for indexing the solution matrix
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;
		Adjlist = (Padjlist *)calloc(Nnodes + 1, sizeof(Padjlist));
		Order = (int *)calloc(Nnodes + 1, sizeof(int));
		Row = (int *)calloc(Nnodes + 1, sizeof(int));
		Ndx = (int *)calloc(Nlinks + 1, sizeof(int));
		ERRCODE(MEMCHECK(Adjlist));
		ERRCODE(MEMCHECK(Order));
		ERRCODE(MEMCHECK(Row));
		ERRCODE(MEMCHECK(Ndx));
		return(errcode);
	}


	void  EPANET::freesparse()
		/*
		**----------------------------------------------------------------
		** Input:   None
		** Output:  None
		** Purpose: Frees memory used for sparse matrix storage
		**----------------------------------------------------------------
		*/
	{
		freelists();
		free(Adjlist);
		free(Order);
		free(Row);
		free(Ndx);
		free(XLNZ);
		free(NZSUB);
		free(LNZ);
	}                        /* End of freesparse */


	int  EPANET::buildlists(int paraflag)
		/*
		**--------------------------------------------------------------
		** Input:   paraflag = TRUE if list marks parallel links
		** Output:  returns error code
		** Purpose: builds linked list of links adjacent to each node
		**--------------------------------------------------------------
		*/
	{
		int    i, j, k;
		int    pmark = 0;
		int    errcode = 0;
		Padjlist  alink;

		/* For each link, update adjacency lists of its end nodes */
		for (k = 1; k <= Nlinks; k++)
		{
			i = Link[k].N1;
			j = Link[k].N2;
			if (paraflag) pmark = paralink(i, j, k);  /* Parallel link check */

													  /* Include link in start node i's list */
			alink = (Sadjlist *)malloc(sizeof(Sadjlist));
			if (alink == NULL) return(101);
			if (!pmark) alink->node = j;
			else        alink->node = 0;           /* Parallel link marker */
			alink->link = k;
			alink->next = Adjlist[i];
			Adjlist[i] = alink;

			/* Include link in end node j's list */
			alink = (Sadjlist *)malloc(sizeof(Sadjlist));
			if (alink == NULL) return(101);
			if (!pmark) alink->node = i;
			else        alink->node = 0;           /* Parallel link marker */
			alink->link = k;
			alink->next = Adjlist[j];
			Adjlist[j] = alink;
		}
		return(errcode);
	}                        /* End of buildlists */


	int  EPANET::paralink(int i, int j, int k)
		/*
		**--------------------------------------------------------------
		** Input:   i = index of start node of link
		**          j = index of end node of link
		**          k = link index
		** Output:  returns 1 if link k parallels another link, else 0
		** Purpose: checks for parallel links between nodes i and j
		**
		**--------------------------------------------------------------
		*/
	{
		Padjlist alink;
		for (alink = Adjlist[i]; alink != NULL; alink = alink->next)
		{
			if (alink->node == j)     /* Link || to k (same end nodes) */
			{
				Ndx[k] = alink->link;  /* Assign Ndx entry to this link */
				return(1);
			}
		}
		Ndx[k] = k;                  /* Ndx entry if link not parallel */
		return(0);
	}                        /* End of paralink */


	void  EPANET::xparalinks()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  none
		** Purpose: removes parallel links from nodal adjacency lists
		**--------------------------------------------------------------
		*/
	{
		int    i;
		Padjlist  alink,       /* Current item in adjacency list */
			blink;       /* Previous item in adjacency list */

						 /* Scan adjacency list of each node */
		for (i = 1; i <= Nnodes; i++)
		{
			alink = Adjlist[i];              /* First item in list */
			blink = NULL;
			while (alink != NULL)
			{
				if (alink->node == 0)      /* Parallel link marker found */
				{
					if (blink == NULL)      /* This holds at start of list */
					{
						Adjlist[i] = alink->next;
						free(alink);             /* Remove item from list */
						alink = Adjlist[i];
					}
					else                    /* This holds for interior of list */
					{
						blink->next = alink->next;
						free(alink);             /* Remove item from list */
						alink = blink->next;
					}
				}
				else
				{
					blink = alink;          /* Move to next item in list */
					alink = alink->next;
				}
			}
		}
	}                        /* End of xparalinks */


	void  EPANET::freelists()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  none
		** Purpose: frees memory used for nodal adjacency lists
		**--------------------------------------------------------------
		*/
	{
		int   i;
		Padjlist alink;

		for (i = 0; i <= Nnodes; i++)
		{
			for (alink = Adjlist[i]; alink != NULL; alink = Adjlist[i])
			{
				Adjlist[i] = alink->next;
				free(alink);
			}
		}
	}                        /* End of freelists */


	void  EPANET::countdegree()
		/*
		**----------------------------------------------------------------
		** Input:   none
		** Output:  none
		** Purpose: counts number of nodes directly connected to each node
		**----------------------------------------------------------------
		*/
	{
		int   i;
		Padjlist alink;
		memset(Degree, 0, (Nnodes + 1) * sizeof(int));

		/* NOTE: For purposes of node re-ordering, Tanks (nodes with  */
		/*       indexes above Njuncs) have zero degree of adjacency. */

		for (i = 1; i <= Njuncs; i++)
			for (alink = Adjlist[i]; alink != NULL; alink = alink->next)
				if (alink->node > 0) Degree[i]++;
	}


	int   EPANET::reordernodes()
		/*
		**--------------------------------------------------------------
		** Input:   none
		** Output:  returns 1 if successful, 0 if not
		** Purpose: re-orders nodes to minimize # of non-zeros that
		**          will appear in factorized solution matrix
		**--------------------------------------------------------------
		*/
	{
		int k, knode, m, n;
		for (k = 1; k <= Nnodes; k++)
		{
			Row[k] = k;
			Order[k] = k;
		}
		n = Njuncs;
		for (k = 1; k <= n; k++)                   /* Examine each junction    */
		{
			m = mindegree(k, n);                 /* Node with lowest degree  */
			knode = Order[m];                   /* Node's index             */
			if (!growlist(knode)) return(101);  /* Augment adjacency list   */
			Order[m] = Order[k];                /* Switch order of nodes    */
			Order[k] = knode;
			Degree[knode] = 0;                  /* In-activate node         */
		}
		for (k = 1; k <= n; k++)                   /* Assign nodes to rows of  */
			Row[Order[k]] = k;                   /*   coeff. matrix          */
		return(0);
	}                        /* End of reordernodes */


	int  EPANET::mindegree(int k, int n)
		/*
		**--------------------------------------------------------------
		** Input:   k = first node in list of active nodes
		**          n = total number of junction nodes
		** Output:  returns node index with fewest direct connections
		** Purpose: finds active node with fewest direct connections
		**--------------------------------------------------------------
		*/
	{
		int i, m;
		int min = n,
			imin = n;

		for (i = k; i <= n; i++)
		{
			m = Degree[Order[i]];
			if (m < min)
			{
				min = m;
				imin = i;
			}
		}
		return(imin);
	}                        /* End of mindegree */


	int  EPANET::growlist(int knode)
		/*
		**--------------------------------------------------------------
		** Input:   knode = node index
		** Output:  returns 1 if successful, 0 if not
		** Purpose: creates new entries in knode's adjacency list for
		**          all unlinked pairs of active nodes that are
		**          adjacent to knode
		**--------------------------------------------------------------
		*/
	{
		int   node;
		Padjlist alink;

		/* Iterate through all nodes connected to knode */
		for (alink = Adjlist[knode]; alink != NULL; alink = alink->next)
		{
			node = alink->node;       /* End node of connecting link  */
			if (Degree[node] > 0)     /* End node is active           */
			{
				Degree[node]--;        /* Reduce degree of adjacency   */
				if (!newlink(alink))   /* Add to adjacency list        */
					return(0);
			}
		}
		return(1);
	}                        /* End of growlist */


	int  EPANET::newlink(Padjlist alink)
		/*
		**--------------------------------------------------------------
		** Input:   alink = element of node's adjacency list
		** Output:  returns 1 if successful, 0 if not
		** Purpose: links end of current adjacent link to end nodes of
		**          all links that follow it on adjacency list
		**--------------------------------------------------------------
		*/
	{
		int   inode, jnode;
		Padjlist blink;

		/* Scan all entries in adjacency list that follow anode. */
		inode = alink->node;             /* End node of connection to anode */
		for (blink = alink->next; blink != NULL; blink = blink->next)
		{
			jnode = blink->node;          /* End node of next connection */

										  /* If jnode still active, and inode not connected to jnode, */
										  /* then add a new connection between inode and jnode.       */
			if (Degree[jnode] > 0)        /* jnode still active */
			{
				if (!linked(inode, jnode))  /* inode not linked to jnode */
				{

					/* Since new connection represents a non-zero coeff. */
					/* in the solution matrix, update the coeff. count.  */
					Ncoeffs++;

					/* Update adjacency lists for inode & jnode to */
					/* reflect the new connection.                 */
					if (!addlink(inode, jnode, Ncoeffs)) return(0);
					if (!addlink(jnode, inode, Ncoeffs)) return(0);
					Degree[inode]++;
					Degree[jnode]++;
				}
			}
		}
		return(1);
	}                        /* End of newlink */


	int  EPANET::linked(int i, int j)
		/*
		**--------------------------------------------------------------
		** Input:   i = node index
		**          j = node index
		** Output:  returns 1 if nodes i and j are linked, 0 if not
		** Purpose: checks if nodes i and j are already linked.
		**--------------------------------------------------------------
		*/
	{
		Padjlist alink;
		for (alink = Adjlist[i]; alink != NULL; alink = alink->next)
			if (alink->node == j) return(1);
		return(0);
	}                        /* End of linked */


	int  EPANET::addlink(int i, int j, int n)
		/*
		**--------------------------------------------------------------
		** Input:   i = node index
		**          j = node index
		**          n = link index
		** Output:  returns 1 if successful, 0 if not
		** Purpose: augments node i's adjacency list with node j
		**--------------------------------------------------------------
		*/
	{
		Padjlist alink;
		alink = (Sadjlist *)malloc(sizeof(Sadjlist));
		if (alink == NULL) return(0);
		alink->node = j;
		alink->link = n;
		alink->next = Adjlist[i];
		Adjlist[i] = alink;
		return(1);
	}                        /* End of addlink */


	int  EPANET::storesparse(int n)
		/*
		**--------------------------------------------------------------
		** Input:   n = number of rows in solution matrix
		** Output:  returns error code
		** Purpose: stores row indexes of non-zeros of each column of
		**          lower triangular portion of factorized matrix
		**--------------------------------------------------------------
		*/
	{
		Padjlist alink;
		int   i, ii, j, k, l, m;
		int   errcode = 0;

		/* Allocate sparse matrix storage */
		XLNZ = (int *)calloc(n + 2, sizeof(int));
		NZSUB = (int *)calloc(Ncoeffs + 2, sizeof(int));
		LNZ = (int *)calloc(Ncoeffs + 2, sizeof(int));
		ERRCODE(MEMCHECK(XLNZ));
		ERRCODE(MEMCHECK(NZSUB));
		ERRCODE(MEMCHECK(LNZ));
		if (errcode) return(errcode);

		/* Generate row index pointers for each column of matrix */
		k = 0;
		XLNZ[1] = 1;
		for (i = 1; i <= n; i++)             /* column */
		{
			m = 0;
			ii = Order[i];
			for (alink = Adjlist[ii]; alink != NULL; alink = alink->next)
			{
				j = Row[alink->node];    /* row */
				l = alink->link;
				if (j > i && j <= n)
				{
					m++;
					k++;
					NZSUB[k] = j;
					LNZ[k] = l;
				}
			}
			XLNZ[i + 1] = XLNZ[i] + m;
		}
		return(errcode);
	}                        /* End of storesparse */


	int  EPANET::ordersparse(int n)
		/*
		**--------------------------------------------------------------
		** Input:   n = number of rows in solution matrix
		** Output:  returns eror code
		** Purpose: puts row indexes in ascending order in NZSUB
		**--------------------------------------------------------------
		*/
	{
		int  i, k;
		int *xlnzt, *nzsubt, *lnzt, *nzt;
		int  errcode = 0;

		xlnzt = (int *)calloc(n + 2, sizeof(int));
		nzsubt = (int *)calloc(Ncoeffs + 2, sizeof(int));
		lnzt = (int *)calloc(Ncoeffs + 2, sizeof(int));
		nzt = (int *)calloc(n + 2, sizeof(int));
		ERRCODE(MEMCHECK(xlnzt));
		ERRCODE(MEMCHECK(nzsubt));
		ERRCODE(MEMCHECK(lnzt));
		ERRCODE(MEMCHECK(nzt));
		if (!errcode)
		{

			/* Count # non-zeros in each row */
			for (i = 1; i <= n; i++) nzt[i] = 0;
			for (i = 1; i <= n; i++)
			{
				for (k = XLNZ[i]; k < XLNZ[i + 1]; k++) nzt[NZSUB[k]]++;
			}
			xlnzt[1] = 1;
			for (i = 1; i <= n; i++) xlnzt[i + 1] = xlnzt[i] + nzt[i];

			/* Transpose matrix twice to order column indexes */
			transpose(n, XLNZ, NZSUB, LNZ, xlnzt, nzsubt, lnzt, nzt);
			transpose(n, xlnzt, nzsubt, lnzt, XLNZ, NZSUB, LNZ, nzt);
		}

		/* Reclaim memory */
		free(xlnzt);
		free(nzsubt);
		free(lnzt);
		free(nzt);
		return(errcode);
	}                        /* End of ordersparse */


	void  EPANET::transpose(int n, int *il, int *jl, int *xl, int *ilt, int *jlt,
		int *xlt, int *nzt)
		/*
		**---------------------------------------------------------------------
		** Input:   n = matrix order
		**          il,jl,xl = sparse storage scheme for original matrix
		**          nzt = work array
		** Output:  ilt,jlt,xlt = sparse storage scheme for transposed matrix
		** Purpose: Determines sparse storage scheme for transpose of a matrix
		**---------------------------------------------------------------------
		*/
	{
		int  i, j, k, kk;

		for (i = 1; i <= n; i++) nzt[i] = 0;
		for (i = 1; i <= n; i++)
		{
			for (k = il[i]; k < il[i + 1]; k++)
			{
				j = jl[k];
				kk = ilt[j] + nzt[j];
				jlt[kk] = i;
				xlt[kk] = xl[k];
				nzt[j]++;
			}
		}
	}                        /* End of transpose */


	int  EPANET::linsolve(int n, double *Aii, double *Aij, double *B)
		/*
		**--------------------------------------------------------------
		** Input:   n    = number of equations
		**          Aii  = diagonal entries of solution matrix
		**          Aij  = non-zero off-diagonal entries of matrix
		**          B    = right hand side coeffs.
		** Output:  B    = solution values
		**          returns 0 if solution found, or index of
		**          equation causing system to be ill-conditioned
		** Purpose: solves sparse symmetric system of linear
		**          equations using Cholesky factorization
		**
		** NOTE:   This procedure assumes that the solution matrix has
		**         been symbolically factorized with the positions of
		**         the lower triangular, off-diagonal, non-zero coeffs.
		**         stored in the following integer arrays:
		**            XLNZ  (start position of each column in NZSUB)
		**            NZSUB (row index of each non-zero in each column)
		**            LNZ   (position of each NZSUB entry in Aij array)
		**
		**  This procedure has been adapted from subroutines GSFCT and
		**  GSSLV in the book "Computer Solution of Large Sparse
		**  Positive Definite Systems" by A. George and J. W-H Liu
		**  (Prentice-Hall, 1981).
		**--------------------------------------------------------------
		*/
	{
		int *link, *first;
		int    i, istop, istrt, isub, j, k, kfirst, newk;
		int    errcode = 0;
		double bj, diagj, ljk;
		double *temp;

		temp = (double *)calloc(n + 1, sizeof(double));
		link = (int *)calloc(n + 1, sizeof(int));
		first = (int *)calloc(n + 1, sizeof(int));
		ERRCODE(MEMCHECK(temp));
		ERRCODE(MEMCHECK(link));
		ERRCODE(MEMCHECK(first));
		if (errcode)
		{
			errcode = -errcode;
			goto ENDLINSOLVE;
		}
		memset(temp, 0, (n + 1) * sizeof(double));
		memset(link, 0, (n + 1) * sizeof(int));

		/* Begin numerical factorization of matrix A into L */
		/*   Compute column L(*,j) for j = 1,...n */
		for (j = 1; j <= n; j++)
		{
			/* For each column L(*,k) that affects L(*,j): */
			diagj = 0.0;
			newk = link[j];
			k = newk;
			while (k != 0)
			{

				/* Outer product modification of L(*,j) by  */
				/* L(*,k) starting at first[k] of L(*,k).   */
				newk = link[k];
				kfirst = first[k];
				ljk = Aij[LNZ[kfirst]];
				diagj += ljk * ljk;
				istrt = kfirst + 1;
				istop = XLNZ[k + 1] - 1;
				if (istop >= istrt)
				{

					/* Before modification, update vectors 'first' */
					/* and 'link' for future modification steps.   */
					first[k] = istrt;
					isub = NZSUB[istrt];
					link[k] = link[isub];
					link[isub] = k;

					/* The actual mod is saved in vector 'temp'. */
					for (i = istrt; i <= istop; i++)
					{
						isub = NZSUB[i];
						temp[isub] += Aij[LNZ[i]] * ljk;
					}
				}
				k = newk;
			}

			/* Apply the modifications accumulated */
			/* in 'temp' to column L(*,j).         */
			diagj = Aii[j] - diagj;
			if (diagj <= 0.0)        /* Check for ill-conditioning */
			{
				errcode = j;
				goto ENDLINSOLVE;
			}
			diagj = sqrt(diagj);
			Aii[j] = diagj;
			istrt = XLNZ[j];
			istop = XLNZ[j + 1] - 1;
			if (istop >= istrt)
			{
				first[j] = istrt;
				isub = NZSUB[istrt];
				link[j] = link[isub];
				link[isub] = j;
				for (i = istrt; i <= istop; i++)
				{
					isub = NZSUB[i];
					bj = (Aij[LNZ[i]] - temp[isub]) / diagj;
					Aij[LNZ[i]] = bj;
					temp[isub] = 0.0;
				}
			}
		}      /* next j */

			   /* Foward substitution */
		for (j = 1; j <= n; j++)
		{
			bj = B[j] / Aii[j];
			B[j] = bj;
			istrt = XLNZ[j];
			istop = XLNZ[j + 1] - 1;
			if (istop >= istrt)
			{
				for (i = istrt; i <= istop; i++)
				{
					isub = NZSUB[i];
					B[isub] -= Aij[LNZ[i]] * bj;
				}
			}
		}

		/* Backward substitution */
		for (j = n; j >= 1; j--)
		{
			bj = B[j];
			istrt = XLNZ[j];
			istop = XLNZ[j + 1] - 1;
			if (istop >= istrt)
			{
				for (i = istrt; i <= istop; i++)
				{
					isub = NZSUB[i];
					bj -= Aij[LNZ[i]] * B[isub];
				}
			}
			B[j] = bj / Aii[j];
		}

	ENDLINSOLVE:
		free(temp);
		free(link);
		free(first);
		return(errcode);
	}                        /* End of linsolve */

	/*quality*/
	int  EPANET::openqual()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: opens WQ solver system
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;
		int n;

		/* Allocate memory pool for WQ segments */
		OutOfMemory = FALSE;
		SegPool = AllocInit();                                                      //(2.00.11 - LR)
		if (SegPool == NULL) errcode = 101;                                         //(2.00.11 - LR)

																					/* Allocate scratch array & reaction rate array*/
		X = (double *)calloc(MAX((Nnodes + 1), (Nlinks + 1)), sizeof(double));
		R = (double *)calloc((Nlinks + 1), sizeof(double));
		ERRCODE(MEMCHECK(X));
		ERRCODE(MEMCHECK(R));

		/* Allocate memory for WQ solver */
		n = Nlinks + Ntanks + 1;
		FirstSeg = (Pseg *)calloc(n, sizeof(Pseg));
		LastSeg = (Pseg *)calloc(n, sizeof(Pseg));
		FlowDir = (char *)calloc(n, sizeof(char));
		n = Nnodes + 1;
		VolIn = (double *)calloc(n, sizeof(double));
		MassIn = (double *)calloc(n, sizeof(double));
		ERRCODE(MEMCHECK(FirstSeg));
		ERRCODE(MEMCHECK(LastSeg));
		ERRCODE(MEMCHECK(FlowDir));
		ERRCODE(MEMCHECK(VolIn));
		ERRCODE(MEMCHECK(MassIn));
		return(errcode);
	}

	/* Local function to compute unit conversion factor for bulk reaction rates */
	double EPANET::getucf(double order)
	{
		if (order < 0.0) order = 0.0;
		if (order == 1.0) return(1.0);
		else return(1. / pow(LperFT3, (order - 1.0)));
	}


	void  EPANET::initqual()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: re-initializes WQ solver system
		**--------------------------------------------------------------
		*/
	{
		int i;

		/* Initialize quality, tank volumes, & source mass flows */
		for (i = 1; i <= Nnodes; i++) C[i] = Node[i].C0;
		for (i = 1; i <= Ntanks; i++) Tank[i].C = Node[Tank[i].Node].C0;
		for (i = 1; i <= Ntanks; i++) Tank[i].V = Tank[i].V0;
		for (i = 1; i <= Nnodes; i++)
			if (Node[i].S != NULL) Node[i].S->Smass = 0.0;

		/* Set WQ parameters */
		Bucf = 1.0;
		Tucf = 1.0;
		Reactflag = 0;
		if (Qualflag != NONE)
		{
			/* Initialize WQ at trace node (if applicable) */
			if (Qualflag == TRACE) C[TraceNode] = 100.0;

			/* Compute Schmidt number */
			if (Diffus > 0.0)
				Sc = Viscos / Diffus;
			else
				Sc = 0.0;

			/* Compute unit conversion factor for bulk react. coeff. */
			Bucf = getucf(BulkOrder);
			Tucf = getucf(TankOrder);

			/* Check if modeling a reactive substance */
			Reactflag = setReactflag();

			/* Reset memory pool */
			FreeSeg = NULL;
			AllocSetPool(SegPool);                                                   //(2.00.11 - LR)
			AllocReset();                                                            //(2.00.11 - LR)
		}

		/* Initialize avg. reaction rates */
		Wbulk = 0.0;
		Wwall = 0.0;
		Wtank = 0.0;
		Wsource = 0.0;

		/* Re-position hydraulics file */
		fseek(HydFile, HydOffset, SEEK_SET);

		/* Set elapsed times to zero */
		Htime = 0;
		Qtime = 0;
		Rtime = Rstart;
		Nperiods = 0;
	}


	int EPANET::runqual(long *t)
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  t = pointer to current simulation time (sec)
		**   Returns: error code
		**   Purpose: retrieves hydraulics for next hydraulic time Step
		**            (at time *t) and saves current results to file
		**--------------------------------------------------------------
		*/
	{
		long    hydtime;       /* Hydraulic solution time */
		long    hydstep;       /* Hydraulic time Step     */
		int     errcode = 0;

		/* Update reported simulation time */
		*t = Qtime;

		/* Read hydraulic solution from hydraulics file */
		if (Qtime == Htime)
		{
			errcode = gethyd(&hydtime, &hydstep);
			Htime = hydtime + hydstep;
		}
		return(errcode);
	}


	int EPANET::nextqual(long *tstep)
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  tstep = pointer to time Step (sec)
		**   Returns: error code
		**   Purpose: updates WQ conditions until next hydraulic
		**            solution occurs (after *tstep secs.)
		**--------------------------------------------------------------
		*/
	{
		long    hydstep;       /* Hydraulic solution time Step */
		int     errcode = 0;

		/* Determine time Step */
		*tstep = 0;
		hydstep = Htime - Qtime;

		/* Perform water quality routing over this time Step */
		if (Qualflag != NONE && hydstep > 0) transport(hydstep);

		/* Update current time */
		if (OutOfMemory) errcode = 101;
		if (!errcode) *tstep = hydstep;
		Qtime += hydstep;

		/* Save final output if no more time steps */
		if (!errcode && Saveflag && *tstep == 0) errcode = savefinaloutput();
		return(errcode);
	}


	int EPANET::stepqual(long *tleft)
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  tleft = pointer to time left in simulation
		**   Returns: error code
		**   Purpose: updates WQ conditions over a single WQ time Step
		**--------------------------------------------------------------
		*/
	{
		long dt, hstep, t, tstep;
		int  errcode = 0;
		tstep = Qstep;
		do
		{
			dt = tstep;
			hstep = Htime - Qtime;
			if (hstep < dt)
			{
				dt = hstep;
				if (Qualflag != NONE) transport(dt);
				Qtime += dt;
				errcode = runqual(&t);
				Qtime = t;
			}
			else
			{
				if (Qualflag != NONE) transport(dt);
				Qtime += dt;
			}
			tstep -= dt;
			if (OutOfMemory) errcode = 101;
		} while (!errcode && tstep > 0);
		*tleft = Dur - Qtime;
		if (!errcode && Saveflag && *tleft == 0) errcode = savefinaloutput();
		return(errcode);
	}


	int EPANET::closequal()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: closes WQ solver system
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;

		/* Free memory pool */
		if (SegPool)                                                              //(2.00.11 - LR)
		{                                                                           //(2.00.11 - LR)
			AllocSetPool(SegPool);                                                 //(2.00.11 - LR)
			AllocFreePool();                                                       //(2.00.11 - LR)
		}                                                                           //(2.00.11 - LR)

		free(FirstSeg);
		free(LastSeg);
		free(FlowDir);
		free(VolIn);
		free(MassIn);
		free(R);
		X = NULL;
		free(X);


		return(errcode);
	}


	int  EPANET::gethyd(long *hydtime, long *hydstep)
		/*
		**-----------------------------------------------------------
		**   Input:   none
		**   Output:  hydtime = pointer to hydraulic solution time
		**            hydstep = pointer to hydraulic time Step
		**   Returns: error code
		**   Purpose: retrieves hydraulic solution and hydraulic
		**            time Step for next hydraulic event
		**
		**   NOTE: when this function is called, WQ results have
		**         already been updated to the point in time when
		**         the next hydraulic event occurs.
		**-----------------------------------------------------------
		*/
	{
		int errcode = 0;

		/* Read hydraulic results from file */
		if (!readhyd(hydtime)) return(307);
		if (!readhydstep(hydstep)) return(307);
		Htime = *hydtime;

		/* Save current results to output file */
		if (Htime >= Rtime)
		{
			if (Saveflag)
			{
				errcode = saveoutput();
				Nperiods++;
			}
			Rtime += Rstep;
		}

		/* If simulating WQ: */
		if (Qualflag != NONE && Qtime < Dur)
		{

			/* Compute reaction rate coeffs. */
			if (Reactflag && Qualflag != AGE) ratecoeffs();

			/* Initialize pipe segments (at time 0) or  */
			/* else re-orient segments if flow reverses.*/
			if (Qtime == 0) initsegs();
			else            reorientsegs();
		}
		return(errcode);
	}


	char  EPANET::setReactflag()
		/*
		**-----------------------------------------------------------
		**   Input:   none
		**   Output:  returns 1 for reactive WQ constituent, 0 otherwise
		**   Purpose: checks if reactive chemical being simulated
		**-----------------------------------------------------------
		*/
	{
		int  i;
		if (Qualflag == TRACE) return(0);
		else if (Qualflag == AGE)   return(1);
		else
		{
			for (i = 1; i <= Nlinks; i++)
			{
				if (Link[i].Type <= PIPE)
				{
					if (Link[i].Kb != 0.0 || Link[i].Kw != 0.0) return(1);
				}
			}
			for (i = 1; i <= Ntanks; i++)
				if (Tank[i].Kb != 0.0) return(1);
		}
		return(0);
	}


	void  EPANET::transport(long tstep)
		/*
		**--------------------------------------------------------------
		**   Input:   tstep = length of current time Step
		**   Output:  none
		**   Purpose: transports constituent mass through pipe network
		**            under a period of constant hydraulic conditions.
		**--------------------------------------------------------------
		*/
	{
		long   qtime, dt;

		/* Repeat until elapsed time equals hydraulic time Step */

		AllocSetPool(SegPool);                                                      //(2.00.11 - LR)
		qtime = 0;
		while (!OutOfMemory && qtime < tstep)
		{                                  /* Qstep is quality time Step */
			dt = MIN(Qstep, tstep - qtime);    /* Current time Step */
			qtime += dt;                    /* Update elapsed time */
			if (Reactflag) updatesegs(dt);  /* Update quality in inner link segs */
			accumulate(dt);                 /* Accumulate flow at nodes */
			updatenodes(dt);                /* Update nodal quality */
			sourceinput(dt);                /* Compute inputs from sources */
			release(dt);                    /* Release new nodal flows */
		}
		updatesourcenodes(tstep);          /* Update quality at source nodes */
	}


	void  EPANET::initsegs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: initializes water quality segments
		**--------------------------------------------------------------
		*/
	{
		int     j, k;
		double   c, v;

		/* Examine each link */
		for (k = 1; k <= Nlinks; k++)
		{

			/* Establish flow direction */
			FlowDir[k] = '+';
			if (Q[k] < 0.) FlowDir[k] = '-';

			/* Set segs to zero */
			LastSeg[k] = NULL;
			FirstSeg[k] = NULL;

			/* Find quality of downstream node */
			j = DOWN_NODE(k);
			if (j <= Njuncs) c = C[j];
			else             c = Tank[j - Njuncs].C;

			/* Fill link with single segment with this quality */
			addseg(k, LINKVOL(k), c);
		}

		/* Initialize segments in tanks that use them */
		for (j = 1; j <= Ntanks; j++)
		{

			/* Skip reservoirs & complete mix tanks */
			if (Tank[j].A == 0.0
				|| Tank[j].MixModel == MIX1) continue;

			/* Tank segment pointers are stored after those for links */
			k = Nlinks + j;
			c = Tank[j].C;
			LastSeg[k] = NULL;
			FirstSeg[k] = NULL;

			/* Add 2 segments for 2-compartment model */
			if (Tank[j].MixModel == MIX2)
			{
				v = MAX(0, Tank[j].V - Tank[j].V1max);
				addseg(k, v, c);
				v = Tank[j].V - v;
				addseg(k, v, c);
			}

			/* Add one segment for FIFO & LIFO models */
			else
			{
				v = Tank[j].V;
				addseg(k, v, c);
			}
		}
	}


	void  EPANET::reorientsegs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: re-orients segments (if flow reverses)
		**--------------------------------------------------------------
		*/
	{
		Pseg   seg, nseg, pseg;
		int    k;
		char   newdir;

		/* Examine each link */
		for (k = 1; k <= Nlinks; k++)
		{

			/* Find new flow direction */
			newdir = '+';
			if (Q[k] == 0.0)     newdir = FlowDir[k];
			else if (Q[k] < 0.0) newdir = '-';

			/* If direction changes, then reverse order of segments */
			/* (first to last) and save new direction */
			if (newdir != FlowDir[k])
			{
				seg = FirstSeg[k];
				FirstSeg[k] = LastSeg[k];
				LastSeg[k] = seg;
				pseg = NULL;
				while (seg != NULL)
				{
					nseg = seg->prev;
					seg->prev = pseg;
					pseg = seg;
					seg = nseg;
				}
				FlowDir[k] = newdir;
			}
		}
	}


	void  EPANET::updatesegs(long dt)
		/*
		**-------------------------------------------------------------
		**   Input:   t = time from last WQ segment update
		**   Output:  none
		**   Purpose: reacts material in pipe segments up to time t
		**-------------------------------------------------------------
		*/
	{
		int    k;
		Pseg   seg;
		double  cseg, rsum, vsum;

		/* Examine each link in network */
		for (k = 1; k <= Nlinks; k++)
		{

			/* Skip zero-length links (pumps & valves) */
			rsum = 0.0;
			vsum = 0.0;
			if (Link[k].Len == 0.0) continue;

			/* Examine each segment of the link */
			seg = FirstSeg[k];
			while (seg != NULL)
			{

				/* React segment over time dt */
				cseg = seg->c;
				seg->c = pipereact(k, seg->c, seg->v, dt);

				/* Accumulate volume-weighted reaction rate */
				if (Qualflag == CHEM)
				{
					rsum += ABS((seg->c - cseg)) * seg->v;
					vsum += seg->v;
				}
				seg = seg->prev;
			}

			/* Normalize volume-weighted reaction rate */
			if (vsum > 0.0) R[k] = rsum / vsum / dt * SECperDAY;
			else R[k] = 0.0;
		}
	}


	void  EPANET::removesegs(int k)
		/*
		**-------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  none
		**   Purpose: removes all segments in link k
		**-------------------------------------------------------------
		*/
	{
		Pseg seg;
		seg = FirstSeg[k];
		while (seg != NULL)
		{
			FirstSeg[k] = seg->prev;
			seg->prev = FreeSeg;
			FreeSeg = seg;
			seg = FirstSeg[k];
		}
		LastSeg[k] = NULL;
	}


	void  EPANET::addseg(int k, double v, double c)
		/*
		**-------------------------------------------------------------
		**   Input:   k = link segment
		**            v = segment volume
		**            c = segment quality
		**   Output:  none
		**   Purpose: adds a segment to start of link k (i.e., upstream
		**            of current last segment).
		**-------------------------------------------------------------
		*/
	{
		Pseg seg;

		if (FreeSeg != NULL)
		{
			seg = FreeSeg;
			FreeSeg = seg->prev;
		}
		else
		{
			seg = (Sseg *)Alloc(sizeof(Sseg));
			if (seg == NULL)
			{
				OutOfMemory = TRUE;
				return;
			}
		}
		seg->v = v;
		seg->c = c;
		seg->prev = NULL;
		if (FirstSeg[k] == NULL) FirstSeg[k] = seg;
		if (LastSeg[k] != NULL) LastSeg[k]->prev = seg;
		LastSeg[k] = seg;
	}


	void EPANET::accumulate(long dt)
		/*
		**-------------------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: accumulates mass flow at nodes and updates nodal
		**            quality
		**-------------------------------------------------------------
		*/
	{
		int    i, j, k;
		double  cseg, v, vseg;
		Pseg   seg;

		/* Re-set memory used to accumulate mass & volume */
		memset(VolIn, 0, (Nnodes + 1) * sizeof(double));
		memset(MassIn, 0, (Nnodes + 1) * sizeof(double));
		memset(X, 0, (Nnodes + 1) * sizeof(double));

		/* Compute average conc. of segments adjacent to each node */
		/* (For use if there is no transport through the node) */
		for (k = 1; k <= Nlinks; k++)
		{
			j = DOWN_NODE(k);             /* Downstream node */
			if (FirstSeg[k] != NULL)      /* Accumulate concentrations */
			{
				MassIn[j] += FirstSeg[k]->c;
				VolIn[j]++;
			}
			j = UP_NODE(k);              /* Upstream node */
			if (LastSeg[k] != NULL)      /* Accumulate concentrations */
			{
				MassIn[j] += LastSeg[k]->c;
				VolIn[j]++;
			}
		}
		for (k = 1; k <= Nnodes; k++)
			if (VolIn[k] > 0.0) X[k] = MassIn[k] / VolIn[k];

		/* Move mass from first segment of each pipe into downstream node */
		memset(VolIn, 0, (Nnodes + 1) * sizeof(double));
		memset(MassIn, 0, (Nnodes + 1) * sizeof(double));
		for (k = 1; k <= Nlinks; k++)
		{
			i = UP_NODE(k);               /* Upstream node */
			j = DOWN_NODE(k);             /* Downstream node */
			v = ABS(Q[k]) * dt;             /* Flow volume */

										  ////  Start of deprecated code segment  ////                                   //(2.00.12 - LR)

										  /* If link volume < flow volume, then transport upstream    */
										  /* quality to downstream node and remove all link segments. */
										  /*      if (LINKVOL(k) < v)
										  {
										  VolIn[j] += v;
										  seg = FirstSeg[k];
										  cseg = C[i];
										  if (seg != NULL) cseg = seg->c;
										  MassIn[j] += v*cseg;
										  removesegs(k);
										  }
										  */
										  /* Otherwise remove flow volume from leading segments */
										  /* and accumulate flow mass at downstream node        */
										  //else

										  ////  End of deprecated code segment.  ////                                    //(2.00.12 - LR)

			while (v > 0.0)                                                          //(2.00.12 - LR)
			{
				/* Identify leading segment in pipe */
				seg = FirstSeg[k];
				if (seg == NULL) break;

				/* Volume transported from this segment is */
				/* minimum of flow volume & segment volume */
				/* (unless leading segment is also last segment) */
				vseg = seg->v;
				vseg = MIN(vseg, v);
				if (seg == LastSeg[k]) vseg = v;

				/* Update volume & mass entering downstream node  */
				cseg = seg->c;
				VolIn[j] += vseg;
				MassIn[j] += vseg * cseg;

				/* Reduce flow volume by amount transported */
				v -= vseg;

				/* If all of segment's volume was transferred, then */
				/* replace leading segment with the one behind it   */
				/* (Note that the current seg is recycled for later use.) */
				if (v >= 0.0 && vseg >= seg->v)
				{
					FirstSeg[k] = seg->prev;
					if (FirstSeg[k] == NULL) LastSeg[k] = NULL;
					seg->prev = FreeSeg;
					FreeSeg = seg;
				}

				/* Otherwise reduce segment's volume */
				else
				{
					seg->v -= vseg;
				}
			}     /* End while */
		}        /* Next link */
	}


	void EPANET::updatenodes(long dt)
		/*
		**---------------------------------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: updates concentration at all nodes to mixture of accumulated
		**            inflow from connecting pipes.
		**
		**  Note:     Does not account for source flow effects. X[i] contains
		**            average concen. of segments adjacent to node i, used in case
		**            there was no inflow into i.
		**---------------------------------------------------------------------------
		*/
	{
		int i;

		/* Update junction quality */
		for (i = 1; i <= Njuncs; i++)
		{
			if (D[i] < 0.0) VolIn[i] -= D[i] * dt;
			if (VolIn[i] > 0.0) C[i] = MassIn[i] / VolIn[i];
			else                C[i] = X[i];
		}

		/* Update tank quality */
		updatetanks(dt);

		/* For flow tracing, set source node concen. to 100. */
		if (Qualflag == TRACE) C[TraceNode] = 100.0;
	}


	void EPANET::sourceinput(long dt)
		/*
		**---------------------------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: computes contribution (if any) of mass additions from WQ
		**            sources at each node.
		**---------------------------------------------------------------------
		*/
	{
		int   j, n;
		double massadded = 0.0, s, volout;
		double qout, qcutoff;
		Psource source;

		/* Establish a flow cutoff which indicates no outflow from a node */
		qcutoff = 10.0 * TINY;

		/* Zero-out the work array X */
		memset(X, 0, (Nnodes + 1) * sizeof(double));
		if (Qualflag != CHEM) return;

		/* Consider each node */
		for (n = 1; n <= Nnodes; n++)
		{

			/* Skip node if no WQ source */
			source = Node[n].S;
			if (source == NULL) continue;
			if (source->C0 == 0.0) continue;

			/* Find total flow volume leaving node */
			if (n <= Njuncs) volout = VolIn[n];  /* Junctions */
			else volout = VolIn[n] - D[n] * dt;    /* Tanks */
			qout = volout / (double)dt;

			/* Evaluate source input only if node outflow > cutoff flow */
			if (qout > qcutoff)
			{

				/* Mass added depends on type of source */
				s = sourcequal(source);
				switch (source->Type)
				{
					/* Concen. Source: */
					/* Mass added = source concen. * -(demand) */
				case CONCEN:

					/* Only add source mass if demand is negative */
					if (D[n] < 0.0)
					{
						massadded = -s * D[n] * dt;

						/* If node is a tank then set concen. to 0. */
						/* (It will be re-set to true value in updatesourcenodes()) */
						if (n > Njuncs) C[n] = 0.0;
					}
					else massadded = 0.0;
					break;

					/* Mass Inflow Booster Source: */
				case MASS:
					massadded = s * dt;
					break;

					/* Setpoint Booster Source: */
					/* Mass added is difference between source */
					/* & node concen. times outflow volume  */
				case SETPOINT:
					if (s > C[n]) massadded = (s - C[n]) * volout;
					else massadded = 0.0;
					break;

					/* Flow-Paced Booster Source: */
					/* Mass added = source concen. times outflow volume */
				case FLOWPACED:
					massadded = s * volout;
					break;
				}

				/* Source concen. contribution = (mass added / outflow volume) */
				X[n] = massadded / volout;

				/* Update total mass added for time period & simulation */
				source->Smass += massadded;
				if (Htime >= Rstart) Wsource += massadded;
			}
		}

		/* Add mass inflows from reservoirs to Wsource*/
		if (Htime >= Rstart)
		{
			for (j = 1; j <= Ntanks; j++)
			{
				if (Tank[j].A == 0.0)
				{
					n = Njuncs + j;
					volout = VolIn[n] - D[n] * dt;
					if (volout > 0.0) Wsource += volout * C[n];
				}
			}
		}
	}


	void EPANET::release(long dt)
		/*
		**---------------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: creates new segments in outflow links from nodes.
		**---------------------------------------------------------
		*/
	{
		int    k, n;
		double  c, q, v;
		Pseg   seg;

		/* Examine each link */
		for (k = 1; k <= Nlinks; k++)
		{

			/* Ignore links with no flow */
			if (Q[k] == 0.0) continue;

			/* Find flow volume released to link from upstream node */
			/* (NOTE: Flow volume is allowed to be > link volume.) */
			n = UP_NODE(k);
			q = ABS(Q[k]);
			v = q * dt;

			/* Include source contribution in quality released from node. */
			c = C[n] + X[n];

			/* If link has a last seg, check if its quality     */
			/* differs from that of the flow released from node.*/
			if ((seg = LastSeg[k]) != NULL)
			{
				/* Quality of seg close to that of node */
				if (ABS(seg->c - c) < Ctol)
				{
					seg->c = (seg->c * seg->v + c * v) / (seg->v + v);                     //(2.00.11 - LR)
					seg->v += v;
				}

				/* Otherwise add a new seg to end of link */
				else addseg(k, v, c);
			}

			/* If link has no segs then add a new one. */
			else addseg(k, LINKVOL(k), c);
		}
	}


	void  EPANET::updatesourcenodes(long dt)
		/*
		**---------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: updates quality at source nodes.
		**            (X[n] = concen. added by source at node n)
		**---------------------------------------------------
		*/
	{
		int i, n;
		Psource source;

		if (Qualflag != CHEM) return;

		/* Examine each WQ source node */
		for (n = 1; n <= Nnodes; n++)
		{
			source = Node[n].S;
			if (source == NULL) continue;

			/* Add source to current node concen. */
			C[n] += X[n];

			/* For tanks, node concen. = internal concen. */
			if (n > Njuncs)
			{
				i = n - Njuncs;
				if (Tank[i].A > 0.0) C[n] = Tank[i].C;
			}

			/* Normalize mass added at source to time Step */
			source->Smass /= (double)dt;
		}
	}


	void  EPANET::updatetanks(long dt)
		/*
		**---------------------------------------------------
		**   Input:   dt = current WQ time Step
		**   Output:  none
		**   Purpose: updates tank volumes & concentrations
		**---------------------------------------------------
		*/
	{
		int   i, n;

		/* Examine each reservoir & tank */
		for (i = 1; i <= Ntanks; i++)
		{

			/* Use initial quality for reservoirs */
			if (Tank[i].A == 0.0)
			{
				n = Tank[i].Node;
				C[n] = Node[n].C0;
			}

			/* Update tank WQ based on mixing model */
			else switch (Tank[i].MixModel)
			{
			case MIX2: tankmix2(i, dt); break;
			case FIFO: tankmix3(i, dt); break;
			case LIFO: tankmix4(i, dt); break;
			default:   tankmix1(i, dt); break;
			}
		}
	}


	////  Deprecated version of tankmix1  ////                                     //(2.00.12 - LR)
	//void  tankmix1(int i, long dt)
	/*
	**---------------------------------------------
	**   Input:   i = tank index
	**            dt = current WQ time Step
	**   Output:  none
	**   Purpose: complete mix tank model
	**---------------------------------------------
	*/
	//{
	//    int   n;
	//    double cin;

	//   /* Blend inflow with contents */
	//   n = Tank[i].Node;
	//   if (VolIn[n] > 0.0) cin = MassIn[n]/VolIn[n];
	//   else                 cin = 0.0;
	//   if (Tank[i].V > 0.0)
	//      Tank[i].C = tankreact(Tank[i].C,Tank[i].V,Tank[i].Kb,dt) +
	//                  (cin - Tank[i].C)*VolIn[n]/Tank[i].V;
	//   else Tank[i].C = cin;
	//   Tank[i].C = MAX(0.0, Tank[i].C);

	//   /* Update tank volume & nodal quality */
	//   Tank[i].V += D[n]*dt;
	//   C[n] = Tank[i].C;
	//}


	////  New version of tankmix1  ////                                            //(2.00.12 - LR)
	void  EPANET::tankmix1(int i, long dt)
		/*
		**---------------------------------------------
		**   Input:   i = tank index
		**            dt = current WQ time Step
		**   Output:  none
		**   Purpose: complete mix tank model
		**---------------------------------------------
		*/
	{
		int   n;
		double cin;
		double c, cmax, vold, vin;

		/* React contents of tank */
		c = tankreact(Tank[i].C, Tank[i].V, Tank[i].Kb, dt);

		/* Determine tank & volumes */
		vold = Tank[i].V;
		n = Tank[i].Node;
		Tank[i].V += D[n] * dt;
		vin = VolIn[n];

		/* Compute inflow concen. */
		if (vin > 0.0) cin = MassIn[n] / vin;
		else           cin = 0.0;
		cmax = MAX(c, cin);

		/* Mix inflow with tank contents */
		if (vin > 0.0) c = (c * vold + cin * vin) / (vold + vin);
		c = MIN(c, cmax);
		c = MAX(c, 0.0);
		Tank[i].C = c;
		C[n] = Tank[i].C;
	}

	/*** Updated 10/25/00 ***/
	////  New version of tankmix2  ////                                            //(2.00.12 - LR) 
	void  EPANET::tankmix2(int i, long dt)
		/*
		**------------------------------------------------
		**   Input:   i = tank index
		**            dt = current WQ time Step
		**   Output:  none
		**   Purpose: 2-compartment tank model
		**            (seg1 = mixing zone,
		**             seg2 = ambient zone)
		**------------------------------------------------
		*/
	{
		int     k, n;
		double  cin,        /* Inflow quality */
			vin,        /* Inflow volume */
			vt,         /* Transferred volume */
			vnet,       /* Net volume change */
			v1max;      /* Full mixing zone volume */
		Pseg     seg1, seg2;  /* Compartment segments */

							  /* Identify segments for each compartment */
		k = Nlinks + i;
		seg1 = LastSeg[k];
		seg2 = FirstSeg[k];
		if (seg1 == NULL || seg2 == NULL) return;

		/* React contents of each compartment */
		seg1->c = tankreact(seg1->c, seg1->v, Tank[i].Kb, dt);
		seg2->c = tankreact(seg2->c, seg2->v, Tank[i].Kb, dt);

		/* Find inflows & outflows */
		n = Tank[i].Node;
		vnet = D[n] * dt;
		vin = VolIn[n];
		if (vin > 0.0) cin = MassIn[n] / vin;
		else           cin = 0.0;
		v1max = Tank[i].V1max;

		/* Tank is filling */
		vt = 0.0;
		if (vnet > 0.0)
		{
			vt = MAX(0.0, (seg1->v + vnet - v1max));
			if (vin > 0.0)
			{
				seg1->c = ((seg1->c) * (seg1->v) + cin * vin) / (seg1->v + vin);
			}
			if (vt > 0.0)
			{
				seg2->c = ((seg2->c) * (seg2->v) + (seg1->c) * vt) / (seg2->v + vt);
			}
		}

		/* Tank is emptying */
		if (vnet < 0.0)
		{
			if (seg2->v > 0.0)
			{
				vt = MIN(seg2->v, (-vnet));
			}
			if (vin + vt > 0.0)
			{
				seg1->c = ((seg1->c) * (seg1->v) + cin * vin + (seg2->c) * vt) /
					(seg1->v + vin + vt);
			}
		}

		/* Update segment volumes */
		if (vt > 0.0)
		{
			seg1->v = v1max;
			if (vnet > 0.0) seg2->v += vt;
			else            seg2->v = MAX(0.0, ((seg2->v) - vt));
		}
		else
		{
			seg1->v += vnet;
			seg1->v = MIN(seg1->v, v1max);
			seg1->v = MAX(0.0, seg1->v);
			seg2->v = 0.0;
		}
		Tank[i].V += vnet;
		Tank[i].V = MAX(0.0, Tank[i].V);

		/* Use quality of mixed compartment (seg1) to */
		/* represent quality of tank since this is where */
		/* outflow begins to flow from */
		Tank[i].C = seg1->c;
		C[n] = Tank[i].C;
	}


	void  EPANET::tankmix3(int i, long dt)
		/*
		**----------------------------------------------------------
		**   Input:   i = tank index
		**            dt = current WQ time Step
		**   Output:  none
		**   Purpose: First-In-First-Out (FIFO) tank model
		**----------------------------------------------------------
		*/
	{
		int   k, n;
		double vin, vnet, vout, vseg;
		double cin, vsum, csum;
		Pseg  seg;

		k = Nlinks + i;
		if (LastSeg[k] == NULL || FirstSeg[k] == NULL) return;

		/* React contents of each compartment */
		if (Reactflag)
		{
			seg = FirstSeg[k];
			while (seg != NULL)
			{
				seg->c = tankreact(seg->c, seg->v, Tank[i].Kb, dt);
				seg = seg->prev;
			}
		}

		/* Find inflows & outflows */
		n = Tank[i].Node;
		vnet = D[n] * dt;
		vin = VolIn[n];
		vout = vin - vnet;
		if (vin > 0.0) cin = MassIn[n] / VolIn[n];
		else           cin = 0.0;
		Tank[i].V += vnet;
		Tank[i].V = MAX(0.0, Tank[i].V);                                            //(2.00.12 - LR)

																					/* Withdraw flow from first segment */
		vsum = 0.0;
		csum = 0.0;
		while (vout > 0.0)
		{
			seg = FirstSeg[k];
			if (seg == NULL) break;
			vseg = seg->v;           /* Flow volume from leading seg */
			vseg = MIN(vseg, vout);
			if (seg == LastSeg[k]) vseg = vout;
			vsum += vseg;
			csum += (seg->c) * vseg;
			vout -= vseg;            /* Remaining flow volume */
			if (vout >= 0.0 && vseg >= seg->v)  /* Seg used up */
			{
				if (seg->prev)                                                        //(2.00.12 - LR)
				{                                                                     //(2.00.12 - LR)
					FirstSeg[k] = seg->prev;
					//if (FirstSeg[k] == NULL) LastSeg[k] = NULL;                      //(2.00.12 - LR)
					seg->prev = FreeSeg;
					FreeSeg = seg;
				}                                                                     //(2.00.12 - LR)
			}
			else                /* Remaining volume in segment */
			{
				seg->v -= vseg;
			}
		}

		/* Use quality withdrawn from 1st segment */
		/* to represent overall quality of tank */
		if (vsum > 0.0) Tank[i].C = csum / vsum;
		else            Tank[i].C = FirstSeg[k]->c;
		C[n] = Tank[i].C;

		/* Add new last segment for new flow entering tank */
		if (vin > 0.0)
		{
			if ((seg = LastSeg[k]) != NULL)
			{
				/* Quality is the same, so just add flow volume to last seg */
				if (ABS(seg->c - cin) < Ctol) seg->v += vin;

				/* Otherwise add a new seg to tank */
				else addseg(k, vin, cin);
			}

			/* If no segs left then add a new one. */
			else addseg(k, vin, cin);
		}
	}


	void  EPANET::tankmix4(int i, long dt)
		/*
		**----------------------------------------------------------
		**   Input:   i = tank index
		**            dt = current WQ time Step
		**   Output:  none
		**   Purpose: Last In-First Out (LIFO) tank model
		**----------------------------------------------------------
		*/
	{
		int   k, n;
		double vin, vnet, cin, vsum, csum, vseg;
		Pseg  seg, tmpseg;

		k = Nlinks + i;
		if (LastSeg[k] == NULL || FirstSeg[k] == NULL) return;

		/* React contents of each compartment */
		if (Reactflag)
		{
			seg = LastSeg[k];
			while (seg != NULL)
			{
				seg->c = tankreact(seg->c, seg->v, Tank[i].Kb, dt);
				seg = seg->prev;
			}
		}

		/* Find inflows & outflows */
		n = Tank[i].Node;
		vnet = D[n] * dt;
		vin = VolIn[n];
		if (vin > 0.0) cin = MassIn[n] / VolIn[n];
		else           cin = 0.0;
		Tank[i].V += vnet;
		Tank[i].V = MAX(0.0, Tank[i].V);                                            //(2.00.12 - LR)
		Tank[i].C = LastSeg[k]->c;

		/* If tank filling, then create new last seg */
		if (vnet > 0.0)
		{
			if ((seg = LastSeg[k]) != NULL)
			{
				/* Quality is the same, so just add flow volume to last seg */
				if (ABS(seg->c - cin) < Ctol) seg->v += vnet;

				/* Otherwise add a new last seg to tank */
				/* which points to old last seg */
				else
				{
					tmpseg = seg;
					LastSeg[k] = NULL;
					addseg(k, vnet, cin);
					LastSeg[k]->prev = tmpseg;
				}
			}

			/* If no segs left then add a new one. */
			else addseg(k, vnet, cin);

			/* Update reported tank quality */
			Tank[i].C = LastSeg[k]->c;
		}

		/* If net emptying then remove last segments until vnet consumed */
		else if (vnet < 0.0)
		{
			vsum = 0.0;
			csum = 0.0;
			vnet = -vnet;
			while (vnet > 0.0)
			{
				seg = LastSeg[k];
				if (seg == NULL) break;
				vseg = seg->v;
				vseg = MIN(vseg, vnet);
				if (seg == FirstSeg[k]) vseg = vnet;
				vsum += vseg;
				csum += (seg->c) * vseg;
				vnet -= vseg;
				if (vnet >= 0.0 && vseg >= seg->v)  /* Seg used up */
				{
					if (seg->prev)                                                     //(2.00.12 - LR)
					{                                                                  //(2.00.12 - LR)
						LastSeg[k] = seg->prev;
						//if (LastSeg[k] == NULL) FirstSeg[k] = NULL;                   //(2.00.12 - LR)
						seg->prev = FreeSeg;
						FreeSeg = seg;
					}                                                                  //(2.00.12 - LR)
				}
				else                /* Remaining volume in segment */
				{
					seg->v -= vseg;
				}
			}
			/* Reported tank quality is mixture of flow released and any inflow */
			Tank[i].C = (csum + MassIn[n]) / (vsum + vin);
		}
		C[n] = Tank[i].C;
	}


	double  EPANET::sourcequal(Psource source)
		/*
		**--------------------------------------------------------------
		**   Input:   j = source index
		**   Output:  returns source WQ value
		**   Purpose: determines source concentration in current time period
		**--------------------------------------------------------------
		*/
	{
		int   i;
		long  k;
		double c;

		/* Get source concentration (or mass flow) in original units */
		c = source->C0;

		/* Convert mass flow rate from min. to sec. */
		/* and convert concen. from liters to cubic feet */
		if (source->Type == MASS) c /= 60.0;
		else c /= Ucf[QUALITY];

		/* Apply time pattern if assigned */
		i = source->Pat;
		if (i == 0) return(c);
		k = ((Qtime + Pstart) / Pstep) % (long)Pattern[i].Length;
		return(c * Pattern[i].F[k]);
	}


	double  EPANET::avgqual(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  returns WQ value
		**   Purpose: computes average quality in link k
		**--------------------------------------------------------------
		*/
	{
		double  vsum = 0.0,
			msum = 0.0;
		Pseg   seg;

		if (Qualflag == NONE) return(0.);
		seg = FirstSeg[k];
		while (seg != NULL)
		{
			vsum += seg->v;
			msum += (seg->c) * (seg->v);
			seg = seg->prev;
		}
		if (vsum > 0.0) return(msum / vsum);
		else return((C[Link[k].N1] + C[Link[k].N2]) / 2.);
	}


	void  EPANET::ratecoeffs()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  none
		**   Purpose: determines wall reaction coeff. for each pipe
		**--------------------------------------------------------------
		*/
	{
		int   k;
		double kw;

		for (k = 1; k <= Nlinks; k++)
		{
			kw = Link[k].Kw;
			if (kw != 0.0) kw = piperate(k);
			Link[k].R = kw;
			R[k] = 0.0;
		}
	}                         /* End of ratecoeffs */


	double EPANET::piperate(int k)
		/*
		**--------------------------------------------------------------
		**   Input:   k = link index
		**   Output:  returns reaction rate coeff. for 1st-order wall
		**            reactions or mass transfer rate coeff. for 0-order
		**            reactions
		**   Purpose: finds wall reaction rate coeffs.
		**--------------------------------------------------------------
		*/
	{
		double a, d, u, kf, kw, y, Re, Sh;

		d = Link[k].Diam;                    /* Pipe diameter, ft */

											 /* Ignore mass transfer if Schmidt No. is 0 */
		if (Sc == 0.0)
		{
			if (WallOrder == 0.0) return(BIG);
			else return(Link[k].Kw * (4.0 / d) / Ucf[ELEV]);
		}

		/* Compute Reynolds No. */
		a = PI * d * d / 4.0;
		u = ABS(Q[k]) / a;
		Re = u * d / Viscos;

		/* Compute Sherwood No. for stagnant flow  */
		/* (mass transfer coeff. = Diffus./radius) */
		if (Re < 1.0) Sh = 2.0;

		/* Compute Sherwood No. for turbulent flow */
		/* using the Notter-Sleicher formula.      */
		else if (Re >= 2300.0)
			Sh = 0.0149 * pow(Re, 0.88) * pow(Sc, 0.333);

		/* Compute Sherwood No. for laminar flow */
		/* using Graetz solution formula.        */
		else
		{
			y = d / Link[k].Len * Re * Sc;
			Sh = 3.65 + 0.0668 * y / (1.0 + 0.04 * pow(y, 0.667));
		}

		/* Compute mass transfer coeff. (in ft/sec) */
		kf = Sh * Diffus / d;

		/* For zero-order reaction, return mass transfer coeff. */
		if (WallOrder == 0.0) return(kf);

		/* For first-order reaction, return apparent wall coeff. */
		kw = Link[k].Kw / Ucf[ELEV];       /* Wall coeff, ft/sec */
		kw = (4.0 / d) * kw * kf / (kf + ABS(kw)); /* Wall coeff, 1/sec  */
		return(kw);
	}                         /* End of piperate */


	double  EPANET::pipereact(int k, double c, double v, long dt)
		/*
		**------------------------------------------------------------
		**   Input:   k = link index
		**            c = current WQ in segment
		**            v = segment volume
		**            dt = time Step
		**   Output:  returns new WQ value
		**   Purpose: computes new quality in a pipe segment after
		**            reaction occurs
		**------------------------------------------------------------
		*/
	{
		double cnew, dc, dcbulk, dcwall, rbulk, rwall;

		/* For water age (hrs), update concentration by timestep */
		if (Qualflag == AGE) return(c + (double)dt / 3600.0);

		/* Otherwise find bulk & wall reaction rates */
		rbulk = bulkrate(c, Link[k].Kb, BulkOrder) * Bucf;
		rwall = wallrate(c, Link[k].Diam, Link[k].Kw, Link[k].R);

		/* Find change in concentration over timestep */
		dcbulk = rbulk * (double)dt;
		dcwall = rwall * (double)dt;

		/* Update cumulative mass reacted */
		if (Htime >= Rstart)
		{
			Wbulk += ABS(dcbulk) * v;
			Wwall += ABS(dcwall) * v;
		}

		/* Update concentration */
		dc = dcbulk + dcwall;
		cnew = c + dc;
		cnew = MAX(0.0, cnew);
		return(cnew);
	}


	double  EPANET::tankreact(double c, double v, double kb, long dt)
		/*
		**-------------------------------------------------------
		**   Input:   c = current WQ in tank
		**            v = tank volume
		**            kb = reaction coeff.
		**            dt = time Step
		**   Output:  returns new WQ value
		**   Purpose: computes new quality in a tank after
		**            reaction occurs
		**-------------------------------------------------------
		*/
	{
		double cnew, dc, rbulk;

		/*** Updated 9/7/00 ***/
		/* If no reaction then return current WQ */
		if (!Reactflag) return(c);

		/* For water age, update concentration by timestep */
		if (Qualflag == AGE) return(c + (double)dt / 3600.0);

		/* Find bulk reaction rate */
		rbulk = bulkrate(c, kb, TankOrder) * Tucf;

		/* Find concentration change & update quality */
		dc = rbulk * (double)dt;
		if (Htime >= Rstart) Wtank += ABS(dc) * v;
		cnew = c + dc;
		cnew = MAX(0.0, cnew);
		return(cnew);
	}


	double  EPANET::bulkrate(double c, double kb, double order)
		/*
		**-----------------------------------------------------------
		**   Input:   c = current WQ concentration
		**            kb = bulk reaction coeff.
		**            order = bulk reaction order
		**   Output:  returns bulk reaction rate
		**   Purpose: computes bulk reaction rate (mass/volume/time)
		**-----------------------------------------------------------
		*/
	{
		double c1;

		/* Find bulk reaction potential taking into account */
		/* limiting potential & reaction order. */

		/* Zero-order kinetics: */
		if (order == 0.0) c = 1.0;

		/* Michaelis-Menton kinetics: */
		else if (order < 0.0)
		{
			c1 = Climit + SGN(kb) * c;
			if (ABS(c1) < TINY) c1 = SGN(c1) * TINY;
			c = c / c1;
		}

		/* N-th order kinetics: */
		else
		{
			/* Account for limiting potential */
			if (Climit == 0.0) c1 = c;
			else c1 = MAX(0.0, SGN(kb) * (Climit - c));

			/* Compute concentration potential */
			if (order == 1.0) c = c1;
			else if (order == 2.0) c = c1 * c;
			else c = c1 * pow(MAX(0.0, c), order - 1.0);
		}

		/* Reaction rate = bulk coeff. * potential) */
		if (c < 0) c = 0;
		return(kb * c);
	}


	double  EPANET::wallrate(double c, double d, double kw, double kf)
		/*
		**------------------------------------------------------------
		**   Input:   c = current WQ concentration
		**            d = pipe diameter
		**            kw = intrinsic wall reaction coeff.
		**            kf = mass transfer coeff. for 0-order reaction
		**                 (ft/sec) or apparent wall reaction coeff.
		**                 for 1-st order reaction (1/sec)
		**   Output:  returns wall reaction rate in mass/ft3/sec
		**   Purpose: computes wall reaction rate
		**------------------------------------------------------------
		*/
	{
		if (kw == 0.0 || d == 0.0) return(0.0);
		if (WallOrder == 0.0)       /* 0-order reaction */
		{
			kf = SGN(kw) * c * kf;       /* Mass transfer rate (mass/ft2/sec)*/
			kw = kw * SQR(Ucf[ELEV]);  /* Reaction rate (mass/ft2/sec) */
			if (ABS(kf) < ABS(kw))   /* Reaction mass transfer limited */
				kw = kf;
			return(kw * 4.0 / d);        /* Reaction rate (mass/ft3/sec) */
		}
		else return(c * kf);          /* 1st-order reaction */
	}

	/*output*/
	int  EPANET::savenetdata()
		/*
		**---------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: saves input data in original units to binary
		**            output file using fixed-sized (4-byte) records
		**---------------------------------------------------------------
		*/
	{
		int   i, nmax;
		INT4 *ibuf;
		REAL4 *x;
		int   errcode = 0;

		/* Allocate buffer arrays */
		nmax = MAX(Nnodes, Nlinks) + 1;
		nmax = MAX(nmax, 15);
		ibuf = (INT4 *)calloc(nmax, sizeof(INT4));
		x = (REAL4 *)calloc(nmax, sizeof(REAL4));
		ERRCODE(MEMCHECK(ibuf));
		ERRCODE(MEMCHECK(x));

		if (!errcode)
		{
			/* Write integer variables to OutFile */
			ibuf[0] = MAGICNUMBER;

			/*** CODEVERSION replaces VERSION ***/                                         //(2.00.11 - LR)
			ibuf[1] = CODEVERSION;                                                   //(2.00.11 - LR)

			ibuf[2] = Nnodes;
			ibuf[3] = Ntanks;
			ibuf[4] = Nlinks;
			ibuf[5] = Npumps;
			ibuf[6] = Nvalves;
			ibuf[7] = Qualflag;
			ibuf[8] = TraceNode;
			ibuf[9] = Flowflag;
			ibuf[10] = Pressflag;
			ibuf[11] = Tstatflag;
			ibuf[12] = Rstart;
			ibuf[13] = Rstep;
			ibuf[14] = Dur;
			fwrite(ibuf, sizeof(INT4), 15, OutFile);

			/* Write string variables to OutFile */
			fwrite(Title[0], sizeof(char), MAXMSG + 1, OutFile);
			fwrite(Title[1], sizeof(char), MAXMSG + 1, OutFile);
			fwrite(Title[2], sizeof(char), MAXMSG + 1, OutFile);
			fwrite(InpFname, sizeof(char), MAXFNAME + 1, OutFile);
			fwrite(Rpt2Fname, sizeof(char), MAXFNAME + 1, OutFile);
			fwrite(ChemName, sizeof(char), MAXID + 1, OutFile);
			fwrite(Field[QUALITY].Units, sizeof(char), MAXID + 1, OutFile);

			/* Write node ID information to OutFile */
			for (i = 1; i <= Nnodes; i++)
				fwrite(Node[i].ID, MAXID + 1, 1, OutFile);

			/* Write link information to OutFile            */
			/* (Note: first transfer values to buffer array,*/
			/* then fwrite buffer array at offset of 1 )    */
			for (i = 1; i <= Nlinks; i++)
				fwrite(Link[i].ID, MAXID + 1, 1, OutFile);
			for (i = 1; i <= Nlinks; i++) ibuf[i] = Link[i].N1;
			fwrite(ibuf + 1, sizeof(INT4), Nlinks, OutFile);
			for (i = 1; i <= Nlinks; i++) ibuf[i] = Link[i].N2;
			fwrite(ibuf + 1, sizeof(INT4), Nlinks, OutFile);
			for (i = 1; i <= Nlinks; i++) ibuf[i] = Link[i].Type;
			fwrite(ibuf + 1, sizeof(INT4), Nlinks, OutFile);

			/* Write tank information to OutFile.*/
			for (i = 1; i <= Ntanks; i++) ibuf[i] = Tank[i].Node;
			fwrite(ibuf + 1, sizeof(INT4), Ntanks, OutFile);
			for (i = 1; i <= Ntanks; i++) x[i] = (REAL4)Tank[i].A;
			FSAVE(Ntanks);

			/* Save node elevations to OutFile.*/
			for (i = 1; i <= Nnodes; i++) x[i] = (REAL4)(Node[i].El * Ucf[ELEV]);
			FSAVE(Nnodes);

			/* Save link lengths & diameters to OutFile.*/
			for (i = 1; i <= Nlinks; i++) x[i] = (REAL4)(Link[i].Len * Ucf[ELEV]);
			FSAVE(Nlinks);
			for (i = 1; i <= Nlinks; i++)
			{
				if (Link[i].Type != PUMP)
					x[i] = (REAL4)(Link[i].Diam * Ucf[DIAM]);
				else
					x[i] = 0.0f;
			}
			if (FSAVE(Nlinks) < (unsigned)Nlinks) errcode = 308;
		}

		/* Free memory used for buffer arrays */
		free(ibuf);
		free(x);
		return(errcode);
	}


	int  EPANET::savehyd(long *htime)
		/*
		**--------------------------------------------------------------
		**   Input:   *htime   = current time
		**   Output:  returns error code
		**   Purpose: saves current hydraulic solution to file HydFile
		**            in binary format
		**--------------------------------------------------------------
		*/
	{
		int i;
		INT4 t;
		int errcode = 0;
		REAL4 *x = (REAL4 *)calloc(MAX(Nnodes, Nlinks) + 1, sizeof(REAL4));
		if (x == NULL) return 101;

		/* Save current time (htime) */
		t = *htime;
		fwrite(&t, sizeof(INT4), 1, HydFile);

		/* Save current nodal demands (D) */
		for (i = 1; i <= Nnodes; i++) x[i] = (REAL4)D[i];
		fwrite(x + 1, sizeof(REAL4), Nnodes, HydFile);

		/* Copy heads (H) to buffer of floats (x) and save buffer */
		for (i = 1; i <= Nnodes; i++) x[i] = (REAL4)H[i];
		fwrite(x + 1, sizeof(REAL4), Nnodes, HydFile);

		/* Force flow in closed links to be zero then save flows */
		for (i = 1; i <= Nlinks; i++)
		{
			if (S[i] <= CLOSED) x[i] = 0.0f;
			else x[i] = (REAL4)Q[i];
		}
		fwrite(x + 1, sizeof(REAL4), Nlinks, HydFile);

		/* Copy link status to buffer of floats (x) & write buffer */
		for (i = 1; i <= Nlinks; i++) x[i] = (REAL4)S[i];
		fwrite(x + 1, sizeof(REAL4), Nlinks, HydFile);

		/* Save link settings & check for successful write-to-disk */
		/* (We assume that if any of the previous fwrites failed,  */
		/* then this one will also fail.) */
		for (i = 1; i <= Nlinks; i++) x[i] = (REAL4)K[i];
		if (fwrite(x + 1, sizeof(REAL4), Nlinks, HydFile) < (unsigned)Nlinks)
			errcode = 308;
		free(x);
		return(errcode);
	}                        /* End of savehyd */


	int  EPANET::savehydstep(long *hydstep)
		/*
		**--------------------------------------------------------------
		**   Input:   *hydstep = next time Step
		**   Output:  returns error code
		**   Purpose: saves next hydraulic timestep to file HydFile
		**            in binary format
		**--------------------------------------------------------------
		*/
	{
		INT4 t;
		int errcode = 0;
		t = *hydstep;
		if (fwrite(&t, sizeof(INT4), 1, HydFile) < 1) errcode = 308;
		if (t == 0) fputc(EOFMARK, HydFile);
		return(errcode);
	}


	int  EPANET::saveenergy()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: saves energy usage by each pump to OutFile
		**            in binary format
		**--------------------------------------------------------------
		*/
	{
		int   i, j;
		INT4  index;
		REAL4 x[6];              /* work array */
		double hdur,             /* total duration in hours */
			t;                /* pumping duration */

		hdur = Dur / 3600.0;
		for (i = 1; i <= Npumps; i++)
		{
			if (hdur == 0.0)
			{
				for (j = 0; j < 5; j++) x[j] = (REAL4)Pump[i].Energy[j];
				x[5] = (REAL4)(Pump[i].Energy[5] * 24.0);
			}
			else
			{
				t = Pump[i].Energy[0];
				x[0] = (REAL4)(t / hdur);
				x[1] = 0.0f;
				x[2] = 0.0f;
				x[3] = 0.0f;
				x[4] = 0.0f;
				if (t > 0.0)
				{
					x[1] = (REAL4)(Pump[i].Energy[1] / t);
					x[2] = (REAL4)(Pump[i].Energy[2] / t);
					x[3] = (REAL4)(Pump[i].Energy[3] / t);
				}
				x[4] = (REAL4)Pump[i].Energy[4];
				x[5] = (REAL4)(Pump[i].Energy[5] * 24.0 / hdur);
			}
			x[0] *= 100.0f;
			x[1] *= 100.0f;
			/* Compute Kw-hr per MilGal (or per cubic meter) */
			if (Unitsflag == SI) x[2] *= (REAL4)(1000.0 / LPSperCFS / 3600.0);
			else                 x[2] *= (REAL4)(1.0e6 / GPMperCFS / 60.0);
			for (j = 0; j < 6; j++) Pump[i].Energy[j] = x[j];
			index = Pump[i].Link;
			if (fwrite(&index, sizeof(INT4), 1, OutFile) < 1) return(308);
			if (fwrite(x, sizeof(REAL4), 6, OutFile) < 6) return(308);
		}
		Emax = Emax * Dcost;
		x[0] = (REAL4)Emax;
		if (fwrite(&x[0], sizeof(REAL4), 1, OutFile) < 1) return(308);
		return(0);
	}


	int  EPANET::readhyd(long *hydtime)
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  *hydtime = time of hydraulic solution
		**   Returns: 1 if successful, 0 if not
		**   Purpose: reads hydraulic solution from file HydFile
		**
		**   NOTE: A hydraulic solution consists of the current time
		**         (hydtime), nodal demands (D) and heads (H), link
		**         flows (Q), link status (S), and link settings (K).
		**--------------------------------------------------------------
		*/
	{
		int   i;
		INT4  t;
		int   result = 1;
		REAL4 *x = (REAL4 *)calloc(MAX(Nnodes, Nlinks) + 1, sizeof(REAL4));
		if (x == NULL) return 0;

		if (fread(&t, sizeof(INT4), 1, HydFile) < 1)  result = 0;
		*hydtime = t;

		if (fread(x + 1, sizeof(REAL4), Nnodes, HydFile) < (unsigned)Nnodes) result = 0;
		else for (i = 1; i <= Nnodes; i++) D[i] = x[i];

		if (fread(x + 1, sizeof(REAL4), Nnodes, HydFile) < (unsigned)Nnodes) result = 0;
		else for (i = 1; i <= Nnodes; i++) H[i] = x[i];

		if (fread(x + 1, sizeof(REAL4), Nlinks, HydFile) < (unsigned)Nlinks) result = 0;
		else for (i = 1; i <= Nlinks; i++) Q[i] = x[i];

		if (fread(x + 1, sizeof(REAL4), Nlinks, HydFile) < (unsigned)Nlinks) result = 0;
		else for (i = 1; i <= Nlinks; i++) S[i] = (char)x[i];

		if (fread(x + 1, sizeof(REAL4), Nlinks, HydFile) < (unsigned)Nlinks) result = 0;
		else for (i = 1; i <= Nlinks; i++) K[i] = x[i];

		free(x);
		return result;
	}                        /* End of readhyd */


	int  EPANET::readhydstep(long *hydstep)
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  *hydstep = next hydraulic time Step (sec)
		**   Returns: 1 if successful, 0 if not
		**   Purpose: reads hydraulic time Step from file HydFile
		**--------------------------------------------------------------
		*/
	{
		INT4  t;
		if (fread(&t, sizeof(INT4), 1, HydFile) < 1)  return(0);
		*hydstep = t;
		return(1);
	}                        /* End of readhydstep */


	int  EPANET::saveoutput()
		/*
		**--------------------------------------------------------------
		**   Input:   none
		**   Output:  returns error code
		**   Purpose: writes simulation results to output file
		**--------------------------------------------------------------
		*/
	{
		int   j;
		int   errcode = 0;
		REAL4 *x = (REAL4 *)calloc(MAX(Nnodes, Nlinks) + 1, sizeof(REAL4));
		if (x == NULL) return 101;

		/* Write out node results, then link results */
		for (j = DEMAND; j <= QUALITY; j++)  ERRCODE(nodeoutput(j, x, Ucf[j]));
		for (j = FLOW; j <= FRICTION; j++) ERRCODE(linkoutput(j, x, Ucf[j]));
		return(errcode);
	}                        /* End of saveoutput */


	int  EPANET::nodeoutput(int j, REAL4 *x, double ucf)
		/*
		**--------------------------------------------------------------
		**   Input:   j   = type of node variable
		**            *x  = buffer for node values
		**            ucf = units conversion factor
		**   Output:  returns error code
		**   Purpose: writes results for node variable j to output file
		**-----------------------------------------------------------------
		*/
	{
		int   i;

		/* Load computed results (in proper units) into buffer x */
		switch (j)
		{
		case DEMAND:    for (i = 1; i <= Nnodes; i++)
			x[i] = (REAL4)(D[i] * ucf);
			break;
		case HEAD:      for (i = 1; i <= Nnodes; i++)
			x[i] = (REAL4)(H[i] * ucf);
			break;
		case PRESSURE:  for (i = 1; i <= Nnodes; i++)
			x[i] = (REAL4)((H[i] - Node[i].El) * ucf);
			break;
		case QUALITY:   for (i = 1; i <= Nnodes; i++)
			x[i] = (REAL4)(C[i] * ucf);
		}

		/* Write x[1] to x[Nnodes] to output file */
		if (fwrite(x + 1, sizeof(REAL4), Nnodes, TmpOutFile) < (unsigned)Nnodes)
			return(308);
		return(0);
	}                        /* End of nodeoutput */


	int  EPANET::linkoutput(int j, REAL4 *x, double ucf)
		/*
		**----------------------------------------------------------------
		**   Input:   j   = type of link variable
		**            *x  = buffer for link values
		**            ucf = units conversion factor
		**   Output:  returns error code
		**   Purpose: writes results for link variable j to output file
		**----------------------------------------------------------------
		*/
	{
		int i;
		double a, h, q, f;

		/* Load computed results (in proper units) into buffer x */
		switch (j)
		{
		case FLOW:      for (i = 1; i <= Nlinks; i++)
			x[i] = (REAL4)(Q[i] * ucf);
			break;
		case VELOCITY:  for (i = 1; i <= Nlinks; i++)
		{
			if (Link[i].Type == PUMP) x[i] = 0.0f;
			else
			{
				q = ABS(Q[i]);
				a = PI * SQR(Link[i].Diam) / 4.0;
				x[i] = (REAL4)(q / a * ucf);
			}
		}
					 break;
		case HEADLOSS:  for (i = 1; i <= Nlinks; i++)
		{
			if (S[i] <= CLOSED) x[i] = 0.0f;
			else
			{
				h = H[Link[i].N1] - H[Link[i].N2];
				if (Link[i].Type != PUMP) h = ABS(h);
				if (Link[i].Type <= PIPE)
					x[i] = (REAL4)(1000.0 * h / Link[i].Len);
				else x[i] = (REAL4)(h * ucf);
			}
		}
					 break;
		case LINKQUAL:  for (i = 1; i <= Nlinks; i++)
			x[i] = (REAL4)(avgqual(i) * ucf);
			break;
		case STATUS:    for (i = 1; i <= Nlinks; i++)
			x[i] = (REAL4)S[i];
			break;
		case SETTING:   for (i = 1; i <= Nlinks; i++)
		{
			if (K[i] != MISSING)
				switch (Link[i].Type)
				{
				case CV:
				case PIPE: x[i] = (REAL4)K[i];
					break;
				case PUMP: x[i] = (REAL4)K[i];
					break;
				case PRV:
				case PSV:
				case PBV:  x[i] = (REAL4)(K[i] * Ucf[PRESSURE]);
					break;
				case FCV:  x[i] = (REAL4)(K[i] * Ucf[FLOW]);
					break;
				case TCV:  x[i] = (REAL4)K[i];
					break;
				default:   x[i] = 0.0f;
				}
			else x[i] = 0.0f;
		}
					break;
		case REACTRATE: /* Overall reaction rate in mass/L/day */
			if (Qualflag == NONE) memset(x, 0, (Nlinks + 1) * sizeof(REAL4));
			else for (i = 1; i <= Nlinks; i++) x[i] = (REAL4)(R[i] * ucf);
			break;
		case FRICTION:   /* f = 2ghd/(Lu^2) where f = friction factor */
						 /* u = velocity, g = grav. accel., h = head  */
						 /*loss, d = diam., & L = pipe length         */
			for (i = 1; i <= Nlinks; i++)
			{
				if (Link[i].Type <= PIPE && ABS(Q[i]) > TINY)
				{
					h = ABS(H[Link[i].N1] - H[Link[i].N2]);
					f = 39.725 * h * pow(Link[i].Diam, 5) / Link[i].Len / SQR(Q[i]);
					x[i] = (REAL4)f;
				}
				else x[i] = 0.0f;
			}
			break;
		}

		/* Write x[1] to x[Nlinks] to output file */
		if (fwrite(x + 1, sizeof(REAL4), Nlinks, TmpOutFile) < (unsigned)Nlinks)
			return(308);
		return(0);
	}                        /* End of linkoutput */


	int  EPANET::savefinaloutput()
		/*
		**--------------------------------------------------------------
		**  Input:   none
		**  Output:  returns error code
		**  Purpose: saves time series statistics, reaction rates &
		**            epilog to output file.
		**--------------------------------------------------------------
		*/
	{
		int errcode = 0;
		REAL4 *x;

		/* Save time series statistic if computed */
		if (Tstatflag != SERIES && TmpOutFile != NULL)
		{
			x = (REAL4 *)calloc(MAX(Nnodes, Nlinks) + 1, sizeof(REAL4));
			if (x == NULL) return 101;
			ERRCODE(savetimestat(x, NODEHDR));
			ERRCODE(savetimestat(x, LINKHDR));
			if (!errcode) Nperiods = 1;
			fclose(TmpOutFile);
			free(x);
		}

		/* Save avg. reaction rates & file epilog */
		if (OutFile != NULL)
		{
			ERRCODE(savenetreacts(Wbulk, Wwall, Wtank, Wsource));
			ERRCODE(saveepilog());
		}
		return(errcode);
	}


	int  EPANET::savetimestat(REAL4 *x, char objtype)
		/*
		**--------------------------------------------------------------
		**   Input:   *x  = buffer for node values
		**            objtype = NODEHDR (for nodes) or LINKHDR (for links)
		**   Output:  returns error code
		**   Purpose: computes time series statistic for nodes or links
		**            and saves to normal output file.
		**
		**   NOTE: This routine is dependent on how the output reporting
		**         variables were assigned to FieldType in TYPES.H.
		**--------------------------------------------------------------
		*/
	{
		int   n, n1, n2;
		int   i, j, p, errcode = 0;
		long  startbyte, skipbytes;
		float *stat1, *stat2, xx;

		/*
		Compute number of bytes in temp output file to skip over (skipbytes)
		when moving from one time period to the next for a particular variable.
		*/
		if (objtype == NODEHDR)
		{
			/*
			For nodes, we start at 0 and skip over node output for all
			node variables minus 1 plus link output for all link variables.
			*/
			startbyte = 0;
			skipbytes = (Nnodes * (QUALITY - DEMAND) +
				Nlinks * (FRICTION - FLOW + 1)) * sizeof(REAL4);
			n = Nnodes;
			n1 = DEMAND;
			n2 = QUALITY;
		}
		else
		{
			/*
			For links, we start at the end of all node variables and skip
			over node output for all node variables plus link output for
			all link variables minus 1.
			*/
			startbyte = Nnodes * (QUALITY - DEMAND + 1) * sizeof(REAL4);
			skipbytes = (Nnodes * (QUALITY - DEMAND + 1) +
				Nlinks * (FRICTION - FLOW)) * sizeof(REAL4);
			n = Nlinks;
			n1 = FLOW;
			n2 = FRICTION;
		}
		stat1 = (float *)calloc(n + 1, sizeof(float));
		stat2 = (float *)calloc(n + 1, sizeof(float));
		ERRCODE(MEMCHECK(stat1));
		ERRCODE(MEMCHECK(stat2));

		/* Process each output reporting variable */
		if (!errcode)
		{
			for (j = n1; j <= n2; j++)
			{

				/* Initialize stat arrays */
				if (Tstatflag == AVG) memset(stat1, 0, (n + 1) * sizeof(float));
				else for (i = 1; i <= n; i++)
				{
					stat1[i] = -MISSING;  /* +1E10 */
					stat2[i] = MISSING;  /* -1E10 */
				}

				/* Position temp output file at start of output */
				fseek(TmpOutFile, startbyte + (j - n1) * n * sizeof(REAL4), SEEK_SET);

				/* Process each time period */
				for (p = 1; p <= Nperiods; p++)
				{

					/* Get output results for time period & update stats */
					fread(x + 1, sizeof(REAL4), n, TmpOutFile);
					for (i = 1; i <= n; i++)
					{
						xx = x[i];
						if (objtype == LINKHDR)
						{
							if (j == FLOW) xx = ABS(xx);
							if (j == STATUS)
							{
								if (xx >= OPEN) xx = 1.0;
								else            xx = 0.0;
							}
						}
						if (Tstatflag == AVG)  stat1[i] += xx;
						else
						{
							stat1[i] = MIN(stat1[i], xx);
							stat2[i] = MAX(stat2[i], xx);
						}
					}

					/* Advance file to next period */
					if (p < Nperiods) fseek(TmpOutFile, skipbytes, SEEK_CUR);
				}

				/* Compute resultant stat & save to regular output file */
				switch (Tstatflag)
				{
				case AVG:   for (i = 1; i <= n; i++) x[i] = stat1[i] / (float)Nperiods;
					break;
				case MIN:   for (i = 1; i <= n; i++) x[i] = stat1[i];
					break;
				case MAX:   for (i = 1; i <= n; i++) x[i] = stat2[i];
					break;
				case RANGE: for (i = 1; i <= n; i++) x[i] = stat2[i] - stat1[i];
					break;
				}
				if (objtype == LINKHDR && j == STATUS)
				{
					for (i = 1; i <= n; i++)
					{
						if (x[i] < 0.5f) x[i] = CLOSED;
						else             x[i] = OPEN;
					}
				}
				if (fwrite(x + 1, sizeof(REAL4), n, OutFile) < (unsigned)n) errcode = 308;

				/* Update internal output variables where applicable */
				if (objtype == NODEHDR) switch (j)
				{
				case DEMAND:  for (i = 1; i <= n; i++) D[i] = x[i] / Ucf[DEMAND];
					break;
				case HEAD:    for (i = 1; i <= n; i++) H[i] = x[i] / Ucf[HEAD];
					break;
				case QUALITY: for (i = 1; i <= n; i++) C[i] = x[i] / Ucf[QUALITY];
					break;
				}
				else if (j == FLOW) for (i = 1; i <= n; i++) Q[i] = x[i] / Ucf[FLOW];
			}
		}

		/* Free allocated memory */
		free(stat1);
		free(stat2);
		return(errcode);
	}


	int  EPANET::savenetreacts(double wbulk, double wwall, double wtank, double wsource)
		/*
		**-----------------------------------------------------
		**  Writes average network-wide reaction rates (in
		**  mass/hr) to binary output file.
		**-----------------------------------------------------
		*/
	{
		int errcode = 0;
		double t;
		REAL4 w[4];
		if (Dur > 0) t = (double)Dur / 3600.;
		else t = 1.;
		w[0] = (REAL4)(wbulk / t);
		w[1] = (REAL4)(wwall / t);
		w[2] = (REAL4)(wtank / t);
		w[3] = (REAL4)(wsource / t);
		if (fwrite(w, sizeof(REAL4), 4, OutFile) < 4) errcode = 308;
		return(errcode);
	}


	int  EPANET::saveepilog()
		/*
		**-------------------------------------------------
		**  Writes Nperiods, Warnflag, & Magic Number to
		**  end of binary output file.
		**-------------------------------------------------
		*/
	{
		int errcode = 0;
		INT4 i;
		i = Nperiods;
		if (fwrite(&i, sizeof(INT4), 1, OutFile) < 1) errcode = 308;
		i = Warnflag;
		if (fwrite(&i, sizeof(INT4), 1, OutFile) < 1) errcode = 308;
		i = MAGICNUMBER;
		if (fwrite(&i, sizeof(INT4), 1, OutFile) < 1) errcode = 308;
		return(errcode);
	}

	/*inpfile*/
	void EPANET::saveauxdata(FILE *f)                                                     //(2.00.12 - LR)
																				   /*
																				   ------------------------------------------------------------
																				   Writes auxilary data from original input file to new file.
																				   ------------------------------------------------------------
																				   */
	{
		int   sect, newsect;
		char *tok;
		char  line[MAXLINE + 1];
		char  s[MAXLINE + 1];

		sect = -1;
		rewind(InFile);
		while (fgets(line, MAXLINE, InFile) != NULL)
		{
			/* Check if line begins with a new section heading */
			strcpy(s, line);
			tok = strtok(s, SEPSTR);
			if (tok != NULL && *tok == '[')
			{
				newsect = findmatch(tok, SectTxt);
				if (newsect >= 0)
				{
					sect = newsect;
					if (sect == _END) break;
					switch (sect)
					{
					case _RULES:
					case _COORDS:
					case _VERTICES:
					case _LABELS:
					case _BACKDROP:
					case _TAGS: fprintf(f, "%s", line);                             //(2.00.12 - LR)
					}
					continue;
				}
				else continue;
			}

			/* Write lines appearing in the section to file */
			switch (sect)
			{
			case _RULES:
			case _COORDS:
			case _VERTICES:
			case _LABELS:
			case _BACKDROP:
			case _TAGS: fprintf(f, "%s", line);                                  //(2.00.12 - LR)
			}
		}
	}


	////  This function was heavily modified.  ////                                //(2.00.12 - LR)

	int  EPANET::saveinpfile(char *fname)
		/*
		-------------------------------------------------
		Writes network data to text file.
		-------------------------------------------------
		*/
	{
		int     i, j, n;
		double  d, kc, ke, km, ucf;
		char    s[MAXLINE + 1], s1[MAXLINE + 1], s2[MAXLINE + 1];
		Pdemand demand;
		Psource source;
		FILE *f;

		/* Open the new text file */

		if ((f = fopen(fname, "wt")) == NULL) return(308);

		/* Write [TITLE] section */

		fprintf(f, "[TITLE]");
		for (i = 0; i < 3; i++)
		{
			if (strlen(Title[i]) > 0) fprintf(f, "\n%s", Title[i]);
		}

		/* Write [JUNCTIONS] section */
		/* (Leave demands for [DEMANDS] section) */

		fprintf(f, "\n\n[JUNCTIONS]");
		for (i = 1; i <= Njuncs; i++)
			fprintf(f, "\n %-31s %12.4f", Node[i].ID, Node[i].El * Ucf[ELEV]);

		/* Write [RESERVOIRS] section */

		fprintf(f, "\n\n[RESERVOIRS]");
		for (i = 1; i <= Ntanks; i++)
		{
			if (Tank[i].A == 0.0)
			{
				n = Tank[i].Node;
				sprintf(s, " %-31s %12.4f", Node[n].ID, Node[n].El * Ucf[ELEV]);
				if ((j = Tank[i].Pat) > 0)
					sprintf(s1, " %-31s", Pattern[j].ID);
				else
					strcpy(s1, "");
				fprintf(f, "\n%s %s", s, s1);
			}
		}

		/* Write [TANKS] section */

		fprintf(f, "\n\n[TANKS]");
		for (i = 1; i <= Ntanks; i++)
		{
			if (Tank[i].A > 0.0)
			{
				n = Tank[i].Node;
				sprintf(s, " %-31s %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f",
					Node[n].ID,
					Node[n].El * Ucf[ELEV],
					(Tank[i].H0 - Node[n].El) * Ucf[ELEV],
					(Tank[i].Hmin - Node[n].El) * Ucf[ELEV],
					(Tank[i].Hmax - Node[n].El) * Ucf[ELEV],
					sqrt(4.0 * Tank[i].A / PI) * Ucf[ELEV],
					Tank[i].Vmin * SQR(Ucf[ELEV]) * Ucf[ELEV]);
				if ((j = Tank[i].Vcurve) > 0)
					sprintf(s1, "%-31s", Curve[j].ID);
				else
					strcpy(s1, "");
				fprintf(f, "\n%s %s", s, s1);
			}
		}

		/* Write [PIPES] section */

		fprintf(f, "\n\n[PIPES]");
		for (i = 1; i <= Nlinks; i++)
		{
			if (Link[i].Type <= PIPE)
			{
				d = Link[i].Diam;
				kc = Link[i].Kc;
				if (Formflag == DW) kc = kc * Ucf[ELEV] * 1000.0;
				km = Link[i].Km * SQR(d) * SQR(d) / 0.02517;
				sprintf(s, " %-31s %-31s %-31s %12.4f %12.4f",
					Link[i].ID,
					Node[Link[i].N1].ID,
					Node[Link[i].N2].ID,
					Link[i].Len * Ucf[LENGTH],
					d * Ucf[DIAM]);
				if (Formflag == DW) sprintf(s1, "%12.4f %12.4f", kc, km);
				else                sprintf(s1, "%12.4f %12.4f", kc, km);
				if (Link[i].Type == CV) sprintf(s2, "CV");
				else if (Link[i].Stat == CLOSED) sprintf(s2, "CLOSED");
				else strcpy(s2, "");
				fprintf(f, "\n%s %s %s", s, s1, s2);
			}
		}

		/* Write [PUMPS] section */

		fprintf(f, "\n\n[PUMPS]");
		for (i = 1; i <= Npumps; i++)
		{
			n = Pump[i].Link;
			sprintf(s, " %-31s %-31s %-31s",
				Link[n].ID,
				Node[Link[n].N1].ID,
				Node[Link[n].N2].ID);

			/* Pump has constant power */
			if (Pump[i].Ptype == CONST_HP)
				sprintf(s1, "  POWER %.4f", Link[n].Km);

			/* Pump has a head curve */
			else if ((j = Pump[i].Hcurve) > 0)
				sprintf(s1, "  HEAD %s", Curve[j].ID);

			/* Old format used for pump curve */
			else
			{
				fprintf(f, "\n%s %12.4f %12.4f %12.4f          0.0 %12.4f", s,
					-Pump[i].H0 * Ucf[HEAD],
					(-Pump[i].H0 - Pump[i].R * pow(Pump[i].Q0, Pump[i].N)) * Ucf[HEAD],
					Pump[i].Q0 * Ucf[FLOW],
					Pump[i].Qmax * Ucf[FLOW]);
				continue;
			}
			strcat(s, s1);

			if ((j = Pump[i].Upat) > 0)
				sprintf(s1, "   PATTERN  %s", Pattern[j].ID);
			else strcpy(s1, "");
			strcat(s, s1);

			if (Link[n].Kc != 1.0)
				sprintf(s1, "  SPEED %.4f", Link[n].Kc);
			else strcpy(s1, "");
			strcat(s, s1);

			fprintf(f, "\n%s", s);
		}

		/* Write [VALVES] section */

		fprintf(f, "\n\n[VALVES]");
		for (i = 1; i <= Nvalves; i++)
		{
			n = Valve[i].Link;
			d = Link[n].Diam;
			kc = Link[n].Kc;
			if (kc == MISSING) kc = 0.0;
			switch (Link[n].Type)
			{
			case FCV: kc *= Ucf[FLOW]; break;
			case PRV:
			case PSV:
			case PBV: kc *= Ucf[PRESSURE]; break;
			}
			km = Link[n].Km * SQR(d) * SQR(d) / 0.02517;

			sprintf(s, " %-31s %-31s %-31s %12.4f %5s",
				Link[n].ID,
				Node[Link[n].N1].ID,
				Node[Link[n].N2].ID,
				d * Ucf[DIAM],
				LinkTxt[Link[n].Type]);

			if (Link[n].Type == GPV && (j = ROUND(Link[n].Kc)) > 0)
				sprintf(s1, "%-31s %12.4f", Curve[j].ID, km);
			else sprintf(s1, "%12.4f %12.4f", kc, km);

			fprintf(f, "\n%s %s", s, s1);
		}

		/* Write [DEMANDS] section */

		fprintf(f, "\n\n[DEMANDS]");
		ucf = Ucf[DEMAND];
		for (i = 1; i <= Njuncs; i++)
		{
			for (demand = Node[i].D; demand != NULL; demand = demand->next)
			{
				sprintf(s, " %-31s %14.6f", Node[i].ID, ucf * demand->Base);
				if ((j = demand->Pat) > 0) sprintf(s1, "   %s", Pattern[j].ID);
				else strcpy(s1, "");
				fprintf(f, "\n%s %s", s, s1);
			}
		}

		/* Write [EMITTERS] section */

		fprintf(f, "\n\n[EMITTERS]");
		for (i = 1; i <= Njuncs; i++)
		{
			if (Node[i].Ke == 0.0) continue;
			ke = Ucf[FLOW] / pow(Ucf[PRESSURE] * Node[i].Ke, (1.0 / Qexp));
			fprintf(f, "\n %-31s %14.6f", Node[i].ID, ke);
		}

		/* Write [STATUS] section */

		fprintf(f, "\n\n[STATUS]");
		for (i = 1; i <= Nlinks; i++)
		{
			if (Link[i].Type <= PUMP)
			{
				if (Link[i].Stat == CLOSED)
					fprintf(f, "\n %-31s %s", Link[i].ID, StatTxt[CLOSED]);

				/* Write pump speed here for pumps with old-style pump curve input */
				else if (Link[i].Type == PUMP)
				{
					n = PUMPINDEX(i);
					if (
						Pump[n].Hcurve == 0 &&
						Pump[n].Ptype != CONST_HP &&
						Link[i].Kc != 1.0
						)
						fprintf(f, "\n %-31s %-.4f", Link[i].ID, Link[i].Kc);
				}
			}

			/* Write fixed-status PRVs & PSVs (setting = MISSING) */
			else if (Link[i].Kc == MISSING)
			{
				if (Link[i].Stat == OPEN)
					fprintf(f, "\n %-31s %s", Link[i].ID, StatTxt[OPEN]);
				if (Link[i].Stat == CLOSED)
					fprintf(f, "\n%-31s %s", Link[i].ID, StatTxt[CLOSED]);
			}
		}

		/* Write [PATTERNS] section */
		/* (Use 6 pattern factors per line) */

		fprintf(f, "\n\n[PATTERNS]");
		for (i = 1; i <= Npats; i++)
		{
			for (j = 0; j < Pattern[i].Length; j++)
			{
				if (j % 6 == 0) fprintf(f, "\n %-31s", Pattern[i].ID);
				fprintf(f, " %12.4f", Pattern[i].F[j]);
			}
		}

		/* Write [CURVES] section */

		fprintf(f, "\n\n[CURVES]");
		for (i = 1; i <= Ncurves; i++)
		{
			for (j = 0; j < Curve[i].Npts; j++)
				fprintf(f, "\n %-31s %12.4f %12.4f",
					Curve[i].ID, Curve[i].X[j], Curve[i].Y[j]);
		}

		/* Write [CONTROLS] section */

		fprintf(f, "\n\n[CONTROLS]");
		for (i = 1; i <= Ncontrols; i++)
		{
			/* Check that controlled link exists */
			if ((j = Control[i].Link) <= 0) continue;

			/* Get text of control's link status/setting */
			if (Control[i].Setting == MISSING)
				sprintf(s, " LINK %s %s ", Link[j].ID, StatTxt[Control[i].Status]);
			else
			{
				kc = Control[i].Setting;
				switch (Link[j].Type)
				{
				case PRV:
				case PSV:
				case PBV: kc *= Ucf[PRESSURE]; break;
				case FCV: kc *= Ucf[FLOW];     break;
				}
				sprintf(s, " LINK %s %.4f", Link[j].ID, kc);
			}

			switch (Control[i].Type)
			{
				/* Print level control */
			case LOWLEVEL:
			case HILEVEL:
				n = Control[i].Node;
				kc = Control[i].Grade - Node[n].El;
				if (n > Njuncs) kc *= Ucf[HEAD];
				else            kc *= Ucf[PRESSURE];
				fprintf(f, "\n%s IF NODE %s %s %.4f", s,
					Node[n].ID, ControlTxt[Control[i].Type], kc);
				break;

				/* Print timer control */
			case TIMER:
				fprintf(f, "\n%s AT %s %.4f HOURS",
					s, ControlTxt[TIMER], Control[i].Time / 3600.);
				break;

				/* Print time-of-day control */
			case TIMEOFDAY:
				fprintf(f, "\n%s AT %s %s",
					s, ControlTxt[TIMEOFDAY], clocktime(Atime, Control[i].Time));
				break;
			}
		}

		/* Write [QUALITY] section */
		/* (Skip nodes with default quality of 0) */

		fprintf(f, "\n\n[QUALITY]");
		for (i = 1; i <= Nnodes; i++)
		{
			if (Node[i].C0 == 0.0) continue;
			fprintf(f, "\n %-31s %14.6f", Node[i].ID, Node[i].C0 * Ucf[QUALITY]);
		}

		/* Write [SOURCES] section */

		fprintf(f, "\n\n[SOURCES]");
		for (i = 1; i <= Nnodes; i++)
		{
			source = Node[i].S;
			if (source == NULL) continue;
			sprintf(s, " %-31s %-8s %14.6f",
				Node[i].ID,
				SourceTxt[source->Type],
				source->C0);
			if ((j = source->Pat) > 0)
				sprintf(s1, "%s", Pattern[j].ID);
			else strcpy(s1, "");
			fprintf(f, "\n%s %s", s, s1);
		}

		/* Write [MIXING] section */

		fprintf(f, "\n\n[MIXING]");
		for (i = 1; i <= Ntanks; i++)
		{
			if (Tank[i].A == 0.0) continue;
			fprintf(f, "\n %-31s %-8s %12.4f",
				Node[Tank[i].Node].ID,
				MixTxt[Tank[i].MixModel],
				(Tank[i].V1max / Tank[i].Vmax));
		}

		/* Write [REACTIONS] section */

		fprintf(f, "\n\n[REACTIONS]");
		fprintf(f, "\n ORDER  BULK            %-.2f", BulkOrder);
		fprintf(f, "\n ORDER  WALL            %-.0f", WallOrder);
		fprintf(f, "\n ORDER  TANK            %-.2f", TankOrder);
		fprintf(f, "\n GLOBAL BULK            %-.6f", Kbulk * SECperDAY);
		fprintf(f, "\n GLOBAL WALL            %-.6f", Kwall * SECperDAY);
		if (Climit > 0.0)
			fprintf(f, "\n LIMITING POTENTIAL     %-.6f", Climit);
		if (Rfactor != MISSING && Rfactor != 0.0)
			fprintf(f, "\n ROUGHNESS CORRELATION  %-.6f", Rfactor);
		for (i = 1; i <= Nlinks; i++)
		{
			if (Link[i].Type > PIPE) continue;
			if (Link[i].Kb != Kbulk)
				fprintf(f, "\n BULK   %-31s %-.6f", Link[i].ID, Link[i].Kb * SECperDAY);
			if (Link[i].Kw != Kwall)
				fprintf(f, "\n WALL   %-31s %-.6f", Link[i].ID, Link[i].Kw * SECperDAY);
		}
		for (i = 1; i <= Ntanks; i++)
		{
			if (Tank[i].A == 0.0) continue;
			if (Tank[i].Kb != Kbulk)
				fprintf(f, "\n TANK   %-31s %-.6f", Node[Tank[i].Node].ID,
					Tank[i].Kb * SECperDAY);
		}

		/* Write [ENERGY] section */

		fprintf(f, "\n\n[ENERGY]");
		if (Ecost != 0.0)
			fprintf(f, "\n GLOBAL PRICE        %-.4f", Ecost);
		if (Epat != 0)
			fprintf(f, "\n GLOBAL PATTERN      %s", Pattern[Epat].ID);
		fprintf(f, "\n GLOBAL EFFIC        %-.4f", Epump);
		fprintf(f, "\n DEMAND CHARGE       %-.4f", Dcost);
		for (i = 1; i <= Npumps; i++)
		{
			if (Pump[i].Ecost > 0.0)
				fprintf(f, "\n PUMP %-31s PRICE   %-.4f",
					Link[Pump[i].Link].ID, Pump[i].Ecost);
			if (Pump[i].Epat > 0.0)
				fprintf(f, "\n PUMP %-31s PATTERN %s",
					Link[Pump[i].Link].ID, Pattern[Pump[i].Epat].ID);
			if (Pump[i].Ecurve > 0.0)
				fprintf(f, "\n PUMP %-31s EFFIC   %s",
					Link[Pump[i].Link].ID, Curve[Pump[i].Ecurve].ID);
		}

		/* Write [TIMES] section */

		fprintf(f, "\n\n[TIMES]");
		fprintf(f, "\n DURATION            %s", clocktime(Atime, Dur));
		fprintf(f, "\n HYDRAULIC TIMESTEP  %s", clocktime(Atime, Hstep));
		fprintf(f, "\n QUALITY TIMESTEP    %s", clocktime(Atime, Qstep));
		fprintf(f, "\n REPORT TIMESTEP     %s", clocktime(Atime, Rstep));
		fprintf(f, "\n REPORT START        %s", clocktime(Atime, Rstart));
		fprintf(f, "\n PATTERN TIMESTEP    %s", clocktime(Atime, Pstep));
		fprintf(f, "\n PATTERN START       %s", clocktime(Atime, Pstart));
		fprintf(f, "\n RULE TIMESTEP       %s", clocktime(Atime, Rulestep));
		fprintf(f, "\n START CLOCKTIME     %s", clocktime(Atime, Tstart));
		fprintf(f, "\n STATISTIC           %s", TstatTxt[Tstatflag]);

		/* Write [OPTIONS] section */

		fprintf(f, "\n\n[OPTIONS]");
		fprintf(f, "\n UNITS               %s", FlowUnitsTxt[Flowflag]);
		fprintf(f, "\n PRESSURE            %s", PressUnitsTxt[Pressflag]);
		fprintf(f, "\n HEADLOSS            %s", FormTxt[Formflag]);
		if (DefPat >= 1 && DefPat <= Npats)
			fprintf(f, "\n PATTERN             %s", Pattern[DefPat].ID);
		if (Hydflag == USE)
			fprintf(f, "\n HYDRAULICS USE      %s", HydFname);
		if (Hydflag == SAVE)
			fprintf(f, "\n HYDRAULICS SAVE     %s", HydFname);
		if (ExtraIter == -1)
			fprintf(f, "\n UNBALANCED          STOP");
		if (ExtraIter >= 0)
			fprintf(f, "\n UNBALANCED          CONTINUE %d", ExtraIter);
		if (Qualflag == CHEM)
			fprintf(f, "\n QUALITY             %s %s", ChemName, ChemUnits);
		if (Qualflag == TRACE)
			fprintf(f, "\n QUALITY             TRACE %-31s", Node[TraceNode].ID);
		if (Qualflag == AGE)
			fprintf(f, "\n QUALITY             AGE");
		if (Qualflag == NONE)
			fprintf(f, "\n QUALITY             NONE");
		fprintf(f, "\n DEMAND MULTIPLIER   %-.4f", Dmult);
		fprintf(f, "\n EMITTER EXPONENT    %-.4f", 1.0 / Qexp);
		fprintf(f, "\n VISCOSITY           %-.6f", Viscos / VISCOS);
		fprintf(f, "\n DIFFUSIVITY         %-.6f", Diffus / DIFFUS);
		fprintf(f, "\n SPECIFIC GRAVITY    %-.6f", SpGrav);
		fprintf(f, "\n TRIALS              %-d", MaxIter);
		fprintf(f, "\n ACCURACY            %-.8f", Hacc);
		fprintf(f, "\n TOLERANCE           %-.8f", Ctol * Ucf[QUALITY]);
		fprintf(f, "\n CHECKFREQ           %-d", CheckFreq);
		fprintf(f, "\n MAXCHECK            %-d", MaxCheck);
		fprintf(f, "\n DAMPLIMIT           %-.8f", DampLimit);

		/* Write [REPORT] section */

		fprintf(f, "\n\n[REPORT]");
		fprintf(f, "\n PAGESIZE            %d", PageSize);
		fprintf(f, "\n STATUS              %s", RptFlagTxt[Statflag]);
		fprintf(f, "\n SUMMARY             %s", RptFlagTxt[Summaryflag]);
		fprintf(f, "\n ENERGY              %s", RptFlagTxt[Energyflag]);
		switch (Nodeflag)
		{
		case 0:
			fprintf(f, "\n NODES               NONE");
			break;
		case 1:
			fprintf(f, "\n NODES               ALL");
			break;
		default:
			j = 0;
			for (i = 1; i <= Nnodes; i++)
			{
				if (Node[i].Rpt == 1)
				{
					if (j % 5 == 0) fprintf(f, "\n NODES               ");
					fprintf(f, "%s ", Node[i].ID);
					j++;
				}
			}
		}
		switch (Linkflag)
		{
		case 0:
			fprintf(f, "\n LINKS               NONE");
			break;
		case 1:
			fprintf(f, "\n LINKS               ALL");
			break;
		default:
			j = 0;
			for (i = 1; i <= Nlinks; i++)
			{
				if (Link[i].Rpt == 1)
				{
					if (j % 5 == 0) fprintf(f, "\n LINKS               ");
					fprintf(f, "%s ", Link[i].ID);
					j++;
				}
			}
		}
		for (i = 0; i < FRICTION; i++)
		{
			if (Field[i].Enabled == TRUE)
			{
				fprintf(f, "\n %-20sPRECISION %d", Field[i].Name, Field[i].Precision);
				if (Field[i].RptLim[LOW] < BIG)
					fprintf(f, "\n %-20sBELOW %.6f", Field[i].Name, Field[i].RptLim[LOW]);
				if (Field[i].RptLim[HI] > -BIG)
					fprintf(f, "\n %-20sABOVE %.6f", Field[i].Name, Field[i].RptLim[HI]);
			}
			else fprintf(f, "\n %-20sNO", Field[i].Name);
		}
		fprintf(f, "\n");

		/* Save auxilary data to new input file */

		saveauxdata(f);

		/* Close the new input file */

		fprintf(f, "\n[END]");
		fclose(f);
		return(0);
	}
}
/*************************** END OF EPANET.C ***************************/