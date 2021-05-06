# rinex2rtcm3 converter

A small RTKLIB-based tool to convert arbitrary number of RINEX files into single RTCMv3 file.

```rinex2rtcm3 [-?|-h|--help] --type <messages type> [--custom <custom types>] --output <output filename> --input <input filenames>```

## Background

It's not the most obvious and required conversion, however, if one is willing to go deep into testing and verification of GNSS receivers, such a tool might become quite handy. 
For example, the testing setup might include GNSS signal simulator or the record&playback device, for which the suitable RTCMv3 RTK (base station) or PPP (reference network) data should be recorded and re-broadcasted.

## Usage

rinex2rtcm3 is based on RTKLIB for the GNSS-related stuff and Lyra (ex-Clara, my favourite CLI library) for the console argument parsing. Input arguments may be separated into five groups:

1. ```--help``` prints the usage info
2. ```--type``` selects the pre-defined set of messages:
    1. ```legacy``` for 1004, 1012, 1019 and 1020 messages (GPS L1+L2 legacy message, GLONASS L1+L2 legacy message, GPS ephemeris, GLONASS ephemeris)
    2. ```compact_msm``` for MSM4 (1074, 1084 etc), 1019, 1020, 1041, 1042, 1044, 1045 and 1046 messages (GPS/GLONASS/Galileo/BeiDou/NavIC/QZSS/SBAS MSM4 and ephemeris)
    3. ```full_msm``` for MSM7 (1077, 1087 etc), 1019, 1020, 1041, 1042, 1044, 1045 and 1046 messages (GPS/GLONASS/Galileo/BeiDou/NavIC/QZSS/SBAS MSM7 and ephemeris)
    4. ```custom_set``` for the messages, provided by the ```--custom``` subcommand
3. ```--custom``` adds *one* message type for conversion. For multiple argument this command should be repeated _N_ times
4. ```--output``` selects the file of the destination RTCMv3 data file
5. ```--input``` selects *one* file as an input. For multiple file this argument should be repeated _N_ times.

## Future work

- [x] Verify build on several platforms
- [x] Add tests (WIP)
- [x] Add CI
- [ ] Tune the CLI
- [ ] Make the file more stream-friendly for long files (interleave ephemeris and observables)
