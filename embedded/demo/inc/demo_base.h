/**
 * @file      demo_base.h
 *
 * @brief     Definition of the components shared by all demonstrations.
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DEMO_BASE_H__
#define __DEMO_BASE_H__

#include "configuration.h"
#include "demo_configuration.h"
#include "lr1110_system.h"
#include "signaling_interface.h"

typedef enum
{
    DEMO_STATUS_PENDING,
    DEMO_STATUS_RUNNING,
    DEMO_STATUS_TERMINATED,
    DEMO_STATUS_STOPPED,
    DEMO_STATUS_SKIPPED,
} demo_status_t;

class DemoBase
{
   public:
    DemoBase( radio_t* radio, SignalingInterface* signaling );
    virtual ~DemoBase( );

    void Initialize( );

    demo_status_t Runtime( );
    static void   InterruptHandler( );

    void         Enable( );
    void         Disable( );
    void         Start( );
    void         Stop( );
    virtual void Reset( );

    static void  ResetAndInitLr1110( const radio_t* radio );
    virtual bool HasIntermediateResults( ) const;

    bool          IsStarted( ) const;
    virtual bool  IsPending( ) const;
    demo_status_t GetStatus( ) const;
    bool          IsWaitingForInterrupt( ) const;

   protected:
    virtual void        SpecificRuntime( )           = 0;
    virtual void        SpecificStop( )              = 0;
    virtual void        SpecificInterruptHandler( )  = 0;
    virtual void        ClearRegisteredIrqs( ) const = 0;
    void                SetWaitingForInterrupt( );
    bool                InterruptHasRaised( );
    void                Terminate( );
    radio_t*            radio;
    SignalingInterface* signaling;

   private:
    static bool          is_initialized;
    bool                 is_waiting_for_interrupt;
    volatile static bool has_received_interrupt;
    static DemoBase*     running_demo;
    bool                 started;
    demo_status_t        status;
};

#endif
