//#include <iostream>
//#include <fstream>
//#include "SLHAline.hh"
//#include "SLHAblock.hh"
//#include "SLHAfile.hh"

#include <iostream>
#include <sstream>


#include "SLHAfile.hh"
#include "SLHAblock.hh"

#undef Complex
#define Complex SS_Complex
#include "linalg.h"
#include "softsusy.h"
#include "lowe.h"
#undef Complex

//awefull but maybe this works
#undef Complex
#define Complex FH_Complex
#include "CFeynHiggs.h"
#include "CSLHA.h"
#include "SLHADefs.h"
#undef Complex


//#undef Complex
//#define Complex MO_Complex
//#include"micromegas_2.4.5/sources/micromegas.h"
////#include"micromegas_2.4.5/sources/micromegas_aux.h"
//#include"micromegas_2.4.5/MSSM/lib/pmodel.h"
//#undef Complex
/**********************************************************************/

static void setFlags()
{
  enum {
    mssmpart = 4,
    fieldren = 0,
    tanbren = 0,
    higgsmix = 2,
    p2approx = 0,
    looplevel = 2,
    runningMT = 1,
    botResum = 1,
    tlCplxApprox = 3
  };

  int error;

  FHSetFlags(&error, mssmpart, fieldren, tanbren, higgsmix,
    p2approx, looplevel, runningMT, botResum, tlCplxApprox);
  if( error ) exit(error);
}


static void setSLHA(const char *filename)
{
  int error;
  COMPLEX slhadata[nslhadata];

  SLHARead(&error, slhadata, filename, 1);
  if( error ) exit(error);

  FHSetSLHA(&error, slhadata);
  if( error ) exit(error);
}


static void higgsCorr()
{
  int error;
  double MHiggs[4];
  FH_Complex SAeff, UHiggs[3][3], ZHiggs[3][3];

  FHHiggsCorr(&error, MHiggs, &SAeff, UHiggs, ZHiggs);
  if( error ) exit(error);

// print some sample output:
  printf("Mh1 = %g\n"
         "Mh2 = %g\n"
         "Mh3 = %g\n"
         "MHp = %g\n"
         "sin alpha_eff = %g %g\n",
    MHiggs[0], MHiggs[1], MHiggs[2], MHiggs[3],
    Re(SAeff), Im(SAeff));
}

/**********************************************************************/


/*--------------------*/
/* BoundaryConditions */
/*--------------------*/
void (*boundaryCondition( int cond ))( MssmSoftsusy &, const DoubleVector &) {
    void (*retval)(MssmSoftsusy &, const DoubleVector &);
    switch (cond) {
        case 0 :
            retval = &sugraBcs;
            break;
        case 1 :
            retval = &extendedSugraBcs;
            break;
        case 2 :
            retval = &generalBcs;
            break;
        case 3 :
            retval = &generalBcs2;
            break;
        case 4 :
            retval = &amsbBcs;
            break;
        case 5 :
            retval = &gmsbBcs;
            break;
        case 6 :
            retval = &splitGmsb;
            break;
        case 7 :
            retval = &lvsBcs;
            break;
        case 8 :
            retval = &nonUniGauginos;
            break;
        case 9 :
            retval = &userDefinedBcs;
            break;
        case 10 :
            retval = &nonUniGauginos;
            break;
        default : 
            std::cout << "Defaulting to sugra boundary conditions" << 
                std::endl;
            retval = &sugraBcs;
    }
    return retval;
}

void MssmSoftsusy_lesHouchesAccordOutputStream( MssmSoftsusy* mss, 
                                                const char model[], 
                                                DoubleVector *pars,
                                                int sgnMu, double tanb,
                                                double qMax, int numPoints,
                                                double mgut, bool altEwsb,
                                                SLHAfile *sf) {

    std::stringstream ss_out( std::stringstream::in |
                              std::stringstream::out );

    mss->lesHouchesAccordOutput( ss_out, model, *pars, sgnMu, tanb, qMax,
                                 numPoints, mgut, altEwsb );
    std::istream iss( ss_out.rdbuf() );
//    std::cout << ss_out.rdbuf();
    iss >> (*sf);
}

int main(void){

//  inputs
    double          top_mass    =173.2;
    softsusy::mass  top_no      =(softsusy::mass)3;
    int             bCond       = 0; // cmssm
    double          mxGuess     = 2e16; // m_gut 
    double          mgut        = 2e16; // m_gut 
    double          qMax        = 91.1875; // Z-mass??
    int             numPoints   = 1;
    int             sgnMu       = 1;
    double          tanb        = 16.26;
    bool            gaugeUnification = false;
    bool            ewsbBCscale = false;
    bool            altEwsb     = false;
    DoubleVector*  pars     = new DoubleVector(3);
    pars->set(1,300.);
    pars->set(2,905.);
    pars->set(3,-1324.);
    char *          model       = "sugra";

   


//  initialisation of objects
///    DoubleVector*  pars= new DoubleVector(200.,100.,0.);
    MssmSoftsusy*  mss      = new MssmSoftsusy();
    QedQcd*        oneset   = new QedQcd();
    SLHAfile *     file    = new SLHAfile;

    oneset->setPoleMt(top_mass);
    oneset->setMass(top_no,top_mass);

    void (*bC)( MssmSoftsusy &, const DoubleVector &) = 
                    boundaryCondition( bCond );
    mss->lowOrg(bC, mxGuess, *pars, sgnMu, tanb, *oneset, 
                    gaugeUnification, ewsbBCscale);
    
//    mss->lesHouchesAccordOutput( std::cout, model, *pars, sgnMu, tanb, qMax,
//                                 numPoints, mgut, altEwsb );
    MssmSoftsusy_lesHouchesAccordOutputStream(  mss, 
                                                model, 
                                                pars,
                                                sgnMu,  tanb,
                                                qMax,  numPoints,
                                                mgut,  altEwsb,
                                                file);     
    SLHAfile deref_file = *file; 
    SLHAblock searchblock;
    searchblock = deref_file["STAUMIX"];
    std::cout << searchblock;

    std::ofstream myfile("files/SLHA-out.txt");
    myfile << *file;
//    std::cout << *file;
    myfile.close();

                            

    setFlags();
    setSLHA("files/SLHA-out.txt");
//    higgsCorr();
    int error;
    double MHiggs[4];
    FH_Complex SAeff, UHiggs[3][3], ZHiggs[3][3];

    FHHiggsCorr(&error, MHiggs, &SAeff, UHiggs, ZHiggs);
    if( error ) exit(error);
    SLHAline FHline;
    FHline.SetNumIndices(1);
    FHline.SetIndex1(25);
    FHline.SetValue(MHiggs[0]);
    FHline.SetComment("FH h0");
    std::cout << FHline;
//  FHcouplings
    FH_Complex couplings[ncouplings], couplingsms[ncouplingsms];
    double gammas[ngammas], gammasms[ngammasms];
    int fast=1;
    FHCouplings(&error,couplings,couplingsms,gammas,gammasms,fast);
    std::cout << couplings[0] << std::endl ;
    
      
    SLHAblock FHblock("FHHiggsMasses");
    FHblock.AddLine(FHline);
    file->AddBlock(FHblock);
    std::ofstream myFHfile("files/SLHA-out-FH-mass.txt");
    myFHfile << *file;

//    int mo_slha_err = readLesH("files/SLHA-out.txt",2);
}
