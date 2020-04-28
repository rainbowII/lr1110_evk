"""
Define job JSON validator classes

 Revised BSD License
 Copyright Semtech Corporation 2020. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Semtech corporation nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

from jsl.fields import StringField, ArrayField, BooleanField, NumberField, OneOfField
from jsl import Document, DocumentField
from lr1110evk.BaseTypes.WifiChannels import WifiChannels

from jsonschema import validate
from json import load


class WiFiChanelField(StringField):
    def __init__(self, *args, **kwargs):
        super(WiFiChanelField, self).__init__(
            pattern="^{}$".format(
                "|".join([chan.name for chan in WifiChannels.WIFI_CHANNELS])
            ),
            *args,
            **kwargs
        )


class WiFiTypeField(StringField):
    def __init__(self, *args, **kwargs):
        super(WiFiTypeField, self).__init__(pattern="^TYPE_(B|G)$")


class WifiApiField(StringField):
    def __init__(self, *args, **kwargs):
        super(WifiApiField, self).__init__(
            pattern="^(wifi_scan|country_code)$", *args, **kwargs
        )


class WifiModeField(StringField):
    def __init__(self, *args, **kwargs):
        super(WifiModeField, self).__init__(pattern="^(beacon_and_packet|beacon_only)$")


class GnssOptionField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssOptionField, self).__init__(pattern="^(best_effort|default)$")


class GnssCaptureModeField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssCaptureModeField, self).__init__(pattern="^(single|dual)$")


class GnssAntennaSelectionField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssAntennaSelectionField, self).__init__(
            pattern="^(no_selection|select_antenna_1|select_antenna_2)$"
        )


class GnssConstellationField(StringField):
    def __init__(self, *args, **kwargs):
        super(GnssConstellationField, self).__init__(pattern="^(gps|beidou)$")


class CommonJobDocument(Document):
    name = StringField()
    n_iterations = NumberField(minimum=0, multiple=1)
    reset_before_job_start = BooleanField()


class WifiJobDocument(CommonJobDocument):
    wifi_api = WifiApiField(required=True)
    wifi_channels = ArrayField(WiFiChanelField())
    wifi_types = ArrayField(WiFiTypeField())
    wifi_nbr_retrial = NumberField(minimum=0, multiple=1)
    wifi_max_result_per_scan = NumberField(minimum=0, multiple=1)
    wifi_timeout = NumberField(minimum=0, multiple=1)
    wifi_mode = WifiModeField()


class AssistedCoordinateDocument(Document):
    latitude = NumberField()
    longitude = NumberField()
    altitude = NumberField()


class GnssAutonomousDocument(CommonJobDocument):
    gnss_autonomous_option = GnssOptionField()
    gnss_autonomous_capture_mode = GnssCaptureModeField()
    gnss_autonomous_nb_satellite = NumberField(multiple_of=1, minimum=0, maximum=255)
    gnss_autonomous_antenna_selection = GnssAntennaSelectionField()
    gnss_autonomous_constellations = ArrayField(GnssConstellationField())


class GnssAssistedDocument(CommonJobDocument):
    gnss_assisted_option = GnssOptionField()
    gnss_assisted_capture_mode = GnssCaptureModeField()
    gnss_assisted_nb_satellite = NumberField(multiple_of=1, minimum=0, maximum=255)
    gnss_assisted_antenna_selection = GnssAntennaSelectionField()
    gnss_assisted_constellations = ArrayField(GnssConstellationField())
    assisted_coordinate = DocumentField(AssistedCoordinateDocument)


class JobsDocument(Document):
    infinite_loops = BooleanField()
    jobs = ArrayField(
        OneOfField(
            fields=[
                DocumentField(WifiJobDocument),
                DocumentField(GnssAutonomousDocument),
                DocumentField(GnssAssistedDocument),
            ]
        )
    )


class JobValidator:
    _schema = JobsDocument.get_schema()

    def __init__(self):
        pass

    @staticmethod
    def validate_job_json_file(json_file):
        with open(json_file, "r") as fp:
            json_stream = load(fp)
        JobValidator.validate_job_json_stream(json_stream)

    @staticmethod
    def validate_job_json_stream(json_stream):
        validate(schema=JobValidator._schema, instance=json_stream)
