#pragma once

#include "rtklib.h"

#include <filesystem>
#include <vector>

namespace {
	extern "C" int showmsg(const char* format, ...) { return 0; }
	extern "C" void settspan(gtime_t ts, gtime_t te) {}
	extern "C" void settime(gtime_t time) {}
	
	// extracted static functions from the streamsvr.c
	
	/* test observation data message ---------------------------------------------*/
	int is_obsmsg(int msg)
	{
		return (1001 <= msg && msg <= 1004) || (1009 <= msg && msg <= 1012) ||
			(1071 <= msg && msg <= 1077) || (1081 <= msg && msg <= 1087) ||
			(1091 <= msg && msg <= 1097) || (1101 <= msg && msg <= 1107) ||
			(1111 <= msg && msg <= 1117) || (1121 <= msg && msg <= 1127) ||
			(1131 <= msg && msg <= 1137);
	}
	/* test navigation data message ----------------------------------------------*/
	int is_navmsg(int msg)
	{
		return msg == 1019 || msg == 1020 || msg == 1044 || msg == 1045 || msg == 1046 ||
			msg == 1042 || msg == 63 || msg == 1041;
	}
	/* test station info message -------------------------------------------------*/
	int is_stamsg(int msg)
	{
		return msg == 1005 || msg == 1006 || msg == 1007 || msg == 1008 || msg == 1033 || msg == 1230;
	}
	/* test time interval --------------------------------------------------------*/
	int is_tint(gtime_t time, double tint)
	{
		if (tint <= 0.0) return 1;
		return fmod(time2gpst(time, NULL) + DTTOL, tint) <= 2.0 * DTTOL;
	}
	/* write rtcm3 msm to stream -------------------------------------------------*/
	void write_rtcm3_msm(stream_t* str, rtcm_t* out, int msg, int sync)
	{
		obsd_t* data, buff[MAXOBS];
		int i, j, n, ns, sys, nobs, code, nsat = 0, nsig = 0, nmsg, mask[MAXCODE] = { 0 };

		if (1071 <= msg && msg <= 1077) sys = SYS_GPS;
		else if (1081 <= msg && msg <= 1087) sys = SYS_GLO;
		else if (1091 <= msg && msg <= 1097) sys = SYS_GAL;
		else if (1101 <= msg && msg <= 1107) sys = SYS_SBS;
		else if (1111 <= msg && msg <= 1117) sys = SYS_QZS;
		else if (1121 <= msg && msg <= 1127) sys = SYS_CMP;
		else if (1131 <= msg && msg <= 1137) sys = SYS_IRN;
		else return;

		data = out->obs.data;
		nobs = out->obs.n;

		/* count number of satellites and signals */
		for (i = 0; i < nobs && i < MAXOBS; i++) {
			if (satsys(data[i].sat, NULL) != sys) continue;
			nsat++;
			for (j = 0; j < NFREQ + NEXOBS; j++) {
				if (!(code = data[i].code[j]) || mask[code - 1]) continue;
				mask[code - 1] = 1;
				nsig++;
			}
		}
		if (nsig > 64) return;

		/* pack data to multiple messages if nsat x nsig > 64 */
		if (nsig > 0) {
			ns = 64 / nsig;         /* max number of sats in a message */
			nmsg = (nsat - 1) / ns + 1; /* number of messages */
		}
		else {
			ns = 0;
			nmsg = 1;
		}
		out->obs.data = buff;

		for (i = j = 0; i < nmsg; i++) {
			for (n = 0; n < ns && j < nobs && j < MAXOBS; j++) {
				if (satsys(data[j].sat, NULL) != sys) continue;
				out->obs.data[n++] = data[j];
			}
			out->obs.n = n;

			if (gen_rtcm3(out, msg, 0, i < nmsg - 1 ? 1 : sync)) {
				strwrite(str, out->buff, out->nbyte);
			}
		}
		out->obs.data = data;
		out->obs.n = nobs;
	}
	/* write obs data messages ---------------------------------------------------*/
	void write_obs(gtime_t time, stream_t* str, strconv_t* conv)
	{
		int i, j = 0;

		for (i = 0; i < conv->nmsg; i++) {
			if (!is_obsmsg(conv->msgs[i]) || !is_tint(time, conv->tint[i])) continue;

			j = i; /* index of last message */
		}
		for (i = 0; i < conv->nmsg; i++) {
			if (!is_obsmsg(conv->msgs[i]) || !is_tint(time, conv->tint[i])) continue;

			/* generate messages */
			if (conv->otype == STRFMT_RTCM2) {
				if (!gen_rtcm2(&conv->out, conv->msgs[i], i != j)) continue;

				/* write messages to stream */
				strwrite(str, conv->out.buff, conv->out.nbyte);
			}
			else if (conv->otype == STRFMT_RTCM3) {
				if (conv->msgs[i] <= 1012) {
					if (!gen_rtcm3(&conv->out, conv->msgs[i], 0, i != j)) continue;
					strwrite(str, conv->out.buff, conv->out.nbyte);
				}
				else { /* write rtcm3 msm to stream */
					write_rtcm3_msm(str, &conv->out, conv->msgs[i], i != j);
				}
			}
		}
	}
	/* write nav data messages ---------------------------------------------------*/
	void write_nav(gtime_t time, stream_t* str, strconv_t* conv)
	{
		int i;

		for (i = 0; i < conv->nmsg; i++) {
			if (!is_navmsg(conv->msgs[i]) || conv->tint[i] > 0.0) continue;

			/* generate messages */
			if (conv->otype == STRFMT_RTCM2) {
				if (!gen_rtcm2(&conv->out, conv->msgs[i], 0)) continue;
			}
			else if (conv->otype == STRFMT_RTCM3) {
				if (!gen_rtcm3(&conv->out, conv->msgs[i], 0, 0)) continue;
			}
			else continue;

			/* write messages to stream */
			strwrite(str, conv->out.buff, conv->out.nbyte);
		}
	}
}

namespace rinex2rtcm3 {
	struct Parameters {
		enum class OutputMessageType {
			CompactMsm,
			FullMsm,
			Legacy,
			CustomSet
		};

		std::vector<std::filesystem::path> input_filenames;
		std::filesystem::path output_filename;
		OutputMessageType message_type = OutputMessageType::CompactMsm;
		std::vector<std::size_t> message_set;

		auto GetTypesString() const {
			std::string dst;
			for (auto& el : message_set)
				dst += std::to_string(el) + ',';
		
			if (!dst.empty())
				dst.resize(dst.size() - 1);

			return dst;
		}
	private:
		void GetOutputMessageSet() {
			switch (message_type) {
			case OutputMessageType::CompactMsm:
				message_set = {1074, 1084, 1094, 1104, 1114, 1124, 1134, 1019, 1020, 1041, 1042, 1044, 1045, 1046, };
				break;
			case OutputMessageType::FullMsm:
				message_set = { 1077, 1087, 1097, 1107, 1117, 1127, 1137, 1019, 1020, 1041, 1042, 1044, 1045, 1046, };
				break;
			case OutputMessageType::Legacy:
				message_set = { 1004, 1012, 1019, 1020, };
				break;
			case OutputMessageType::CustomSet: 
			default:
				break;
			}
		}
	};

	class Converter {
		const Parameters& parameters;
		prcopt_t prcopt = prcopt_default;
		std::string message_types_string;
		std::unique_ptr<strconv_t> conversion_stream;
		std::unique_ptr<strconv_t> conversion_to_write;

		/* copy received data from receiver raw to rtcm ------------------------------*/
		static void raw2rtcm(rtcm_t* out, const raw_t* raw, int ret) {
			out->time = raw->time;
			int i, sat, prn;

			if (ret == 1) {
				for (int i = 0; i < raw->obs.n; i++) {
					out->time = raw->obs.data[i].time;
					out->obs.data[i] = raw->obs.data[i];
				}
				out->obs.n = raw->obs.n;
			}
			else if (ret == 2) {
				sat = raw->ephsat;
				switch (satsys(sat, &prn)) {
				case SYS_GLO: out->nav.geph[prn - 1] = raw->nav.geph[prn - 1]; break;
				case SYS_GPS:
				case SYS_GAL:
				case SYS_QZS:
				case SYS_CMP:
				case SYS_IRN:
					out->nav.eph[sat - 1] = raw->nav.eph[sat - 1]; break;
				}
				out->ephsat = sat;
			}
		}

		
		 auto OpenStream(const std::filesystem::path& path) const {
			auto dst = new stream_t();
			int rw = STR_MODE_W;

			if (!stropen(dst, STR_FILE, rw, path.string().c_str())) {
				strclose(dst);
				throw std::runtime_error("Unable to open output stream");
			}
			return dst;
		}
		
		void Read() const {
			auto& raw = conversion_stream->raw;

			gtime_t ts{};
			gtime_t te{};
			obs_t& obs = raw.obs;
			auto& nav = raw.nav;
			sta_t& sta = raw.sta;
			obs.data = nullptr;	obs.n = obs.nmax = 0;
			nav.eph = nullptr;	nav.n = nav.nmax = 0;
			nav.geph = nullptr;	nav.ng = nav.ngmax = 0;
			nav.seph = nullptr;	nav.ns = nav.nsmax = 0;

			for(auto& el : parameters.input_filenames)
				readrnxt(el.string().c_str(), 0, ts, te, 1, prcopt.rnxopt[0], &obs, &nav, &sta);
		}

		void WriteEphemeris(stream_t* output_stream) {
			auto& raw = conversion_stream->raw;
			auto& nav = raw.nav;
			conversion_to_write->raw.ephsat = nav.n;
			for (auto i = 0; i < nav.n; ++i) {
				conversion_to_write->raw.ephsat = nav.eph[i].sat;
				conversion_to_write->raw.nav.eph[nav.eph[i].sat - 1] = nav.eph[i];
				raw2rtcm(&conversion_to_write->out, &conversion_to_write->raw, 2);
				write_nav(gtime_t(), output_stream, conversion_to_write.get());
			}
		 }
		void WriteObservables(stream_t* output_stream) {
			auto& raw = conversion_stream->raw;
			auto& obs = raw.obs;
			
			auto& obs_to_write = conversion_to_write->raw.obs;
			obs_to_write.n = 0;
			const auto total_number = obs.n;

			if (!total_number)
				return;

			auto start_time = obs.data[0].time;
			
			for (int i = 0; i < total_number; ++i) {
				const auto delta_time = timediff(obs.data[i].time, start_time);
				if (delta_time < std::numeric_limits<double>::epsilon()) {
					obs_to_write.data[obs_to_write.n++] = obs.data[i];
				}
				else {
					start_time = obs.data[i].time;
					raw2rtcm(&conversion_to_write->out, &conversion_to_write->raw, 1);
					write_obs(conversion_to_write->out.time, output_stream, conversion_to_write.get());
					obs_to_write.n = 0;
					obs_to_write.data[obs_to_write.n++] = obs.data[i];
				}
			}
			if (obs_to_write.n) {
				raw2rtcm(&conversion_to_write->out, &conversion_to_write->raw, 1);
				write_obs(conversion_to_write->out.time, output_stream, conversion_to_write.get());
			}

		}
	public:
		Converter(const Parameters& p) :
											parameters(p),
											message_types_string("1077"),
											//message_types_string(p.GetTypesString()),
											conversion_stream(strconvnew(STRFMT_RINEX, STRFMT_RTCM3, message_types_string.c_str(), 0, 0, prcopt.rnxopt[0])),
											conversion_to_write(std::make_unique<strconv_t>()) {
			prcopt.navsys = SYS_ALL;
		}

		void Process() {
			Read();

			auto output_stream = std::unique_ptr<stream_t, decltype(&strclose)>(OpenStream(parameters.output_filename), strclose);
			*conversion_to_write = *conversion_stream;

			WriteEphemeris(output_stream.get());
			WriteObservables(output_stream.get());
		}
	};
}