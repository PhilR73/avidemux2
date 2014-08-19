/***************************************************************************
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <math.h>

#include "Q_processing.h"

#include "ADM_vidMisc.h"

#include "ADM_toolkitQt.h"
#include "DIA_coreToolkit.h"

#if 1
#define aprintf printf
#else
#define aprintf(...) {}
#endif
namespace ADM_Qt4CoreUIToolkit
{
/**
 * \fn ctor
 * @param title
 * @param fps1000
 * @param duration
 */
DIA_processingQt4::DIA_processingQt4(const char *title,uint64_t totalToProcess) : DIA_processingBase(title,totalToProcess)
{
        _totalToProcess=totalToProcess;

        if(!_totalToProcess) _totalToProcess=1000000;
        ui=new Ui_DialogProcessing();
        ui->setupUi(this);
	qtRegisterDialog(this);

        setWindowTitle(title);
        postCtor(); 
}
#define REFRESH_RATE_IN_MS 1000
/**
 * \fn postCtor
 */
void DIA_processingQt4 :: postCtor( void )
{
        _priv=NULL;
        _stopRequest=false;
      
        _nextUpdate=_clock.getElapsedMS()+REFRESH_RATE_IN_MS; // every one sec
        _lastFrames=0;
        _currentFrames=0;      
      
        setWindowModality(Qt::ApplicationModal);        
        
        connect( ui->cancelButton,SIGNAL(clicked(bool)),this,SLOT(stop(bool)));
        ui->labelTimeLeft->setText(QString("00:00:00"));
        ui->progressBar->setValue((int)0);        
        show();
}

/**
 * \fn update
 * @param percent
 * @return 
 */
bool DIA_processingQt4::update(uint32_t frame,uint64_t currentProcess)
{
        UI_purge();

        if(_stopRequest) return true;
        if(!frame) return false;
        
        _currentFrames+=frame;
        uint32_t elapsed=_clock.getElapsedMS();
        if(elapsed<_nextUpdate) 
        {
          return false;
        }
        _clock.reset();
        _nextUpdate=REFRESH_RATE_IN_MS;

        // compute time left
        double percent=(double)(currentProcess)/(double)_totalToProcess;
        
        
        double dElapsed=_totalTime.getElapsedMS(); // in dElapsed time, we have made percent percent
        
        double totalTimeNeeded=dElapsed/percent;
        double remaining=totalTimeNeeded-dElapsed;
        if(remaining<0) remaining=1;
        
        uint32_t hh,mm,ss,mms;
        char string[25];

        ms2time((uint32_t)remaining,&hh,&mm,&ss,&mms);
        sprintf(string,"%02d:%02d:%02d",hh,mm,ss);

        percent=100.*percent;
        aprintf("Percent=%d,cur=%d,tot=%d\n",(int)percent,_lastFrames,_totalFrame);
        aprintf("time %llx/ total time %llx\n",currentProcess,_totalToProcess);
        if(percent>100.) percent=99.9;
        if(percent<0.1) percent=0.1;
        _lastFrames+=_currentFrames;
        _currentFrames=0;
        if(ui)
        {
                        ui->labelImages->setText( QString::number(_lastFrames));
                        ui->labelTimeLeft->setText(QString(string));
                        ui->progressBar->setValue((int)percent);
            
        }
        return false;
}

/**
 * \fn dtor
 */
DIA_processingQt4::~DIA_processingQt4()
{
     qtUnregisterDialog(this);
}


/**
    \fn createWorking
*/
DIA_processingBase *createProcessing(const char *title,uint64_t totalToProcess)
{
    return new DIA_processingQt4(title,totalToProcess);
}   
/**
 * \fn stop
 * @param a
 */
void            DIA_processingQt4::stop(bool a)
        {
            ADM_info("Stop Request\n");
            _stopRequest=true;
        }
}
//********************************************
//EOF