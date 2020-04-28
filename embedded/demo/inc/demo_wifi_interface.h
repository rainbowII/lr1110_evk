/**
 * @file      demo_wifi_interface.h
 *
 * \brief    Interface class for the definition of the Wi-Fi demonstrations.
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

#ifndef __DEMO_WIFI_INTERFACE_H__
#define __DEMO_WIFI_INTERFACE_H__

#include "demo_base.h"
#include "lr1110_wifi.h"
#include <stdint.h>

#define DEMO_WIFI_MAX_RESULT_TOTAL 32

typedef enum
{
    DEMO_WIFI_INIT,
    DEMO_WIFI_SCAN,
    DEMO_WIFI_WAIT_FOR_SCAN,
    DEMO_WIFI_GET_RESULTS,
    DEMO_WIFI_TERMINATED,
} demo_wifi_state_t;

typedef struct
{
    lr1110_wifi_mac_address_t        mac_address;
    lr1110_wifi_channel_t            channel;
    lr1110_wifi_signal_type_result_t type;
    int8_t                           rssi;
    uint8_t                          country_code[LR1110_WIFI_STR_COUNTRY_CODE_SIZE];
} demo_wifi_scan_single_result_t;

typedef struct
{
    uint8_t                          nbrResults;
    demo_wifi_scan_single_result_t   results[DEMO_WIFI_MAX_RESULT_TOTAL];
    lr1110_wifi_cumulative_timings_t timings;
    uint32_t                         global_consumption_uas;
    bool                             error;
} demo_wifi_scan_all_results_t;

class DemoWifiInterface : public DemoBase
{
   public:
    explicit DemoWifiInterface( radio_t* radio, SignalingInterface* signaling );
    virtual ~DemoWifiInterface( );

    virtual void Reset( );
    virtual void SpecificRuntime( );
    virtual void SpecificStop( );
    virtual void SpecificInterruptHandler( );

    const demo_wifi_scan_all_results_t* GetResult( ) const;
    static const char*                  state2str( const demo_wifi_state_t state );

   protected:
    virtual void ClearRegisteredIrqs( ) const;

    virtual void ExecuteScan( radio_t* radio )         = 0;
    virtual void FetchAndSaveResults( radio_t* radio ) = 0;

    /*!
     * \brief Compute consumption based on cumulative timings
     *
     * \return Consumption in micro ampere second (uas)
     */
    static uint32_t              ComputeConsumption( const lr1110_system_reg_mode_t          regMode,
                                                     const lr1110_wifi_cumulative_timings_t& timing );
    demo_wifi_scan_all_results_t results;

   private:
    demo_wifi_state_t state;
    uint32_t          wifi_irq;
};

#endif  // __DEMO_WIFI_INTERFACE_H__
