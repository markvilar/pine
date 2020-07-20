#pragma once

#include <chrono>
#include <string>

#include <boost/serialization/array.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

#include <sl/Camera.hpp>

// Allows for 'save' and 'load' templates rather than 'serialize'
// for non-intrusive serialization. SPLIT = load/save, FREE = non-intrusive
BOOST_SERIALIZATION_SPLIT_FREE(sl::Mat)
BOOST_SERIALIZATION_SPLIT_FREE(sl::String)

namespace boost { 
namespace serialization {

// ----------------------------------- sl::Mat ---------------------------------

template <class Archive>
void save(Archive& ar, const sl::Mat& m, const unsigned int)
{
	size_t width = m.getWidth();
	size_t height = m.getHeight();
	size_t pixel_bytes = m.getPixelBytes();
	auto type = m.getDataType();

	ar & width;
	ar & height;
	ar & pixel_bytes;
	ar & type;

	const size_t data_size = width * height * pixel_bytes;

	ar & boost::serialization::make_array(m.getPtr<sl::uchar1>(),
		data_size);
}

template <class Archive>
void load(Archive& ar, sl::Mat& m, const unsigned int)
{
	size_t width, height, pixel_bytes;
	sl::MAT_TYPE type;

	ar & width;
	ar & height;
	ar & pixel_bytes;
	ar & type;
	
	m.alloc(width, height, type);

	const size_t data_size = width * height * pixel_bytes;

	ar & boost::serialization::make_array(m.getPtr<sl::uchar1>(), 
		data_size);
}


// ----------------------------- sl::InitParameters ----------------------------

template <class Archive>
void serialize(
	Archive& ar, 
	sl::InitParameters& ips, 
	const unsigned int
	)
{
	ar & ips.camera_resolution;
	ar & ips.camera_fps;
	ar & ips.camera_image_flip;
	ar & ips.camera_disable_self_calib;
	ar & ips.enable_right_side_measure;
	ar & ips.svo_real_time_mode;
	ar & ips.depth_mode;
	ar & ips.depth_stabilization;
	ar & ips.depth_minimum_distance;
	ar & ips.depth_maximum_distance;
	ar & ips.coordinate_units;
	ar & ips.coordinate_system;
	ar & ips.sdk_gpu_id;
	ar & ips.sdk_verbose;
	ar & ips.sdk_verbose_log_file;
	ar & ips.input;
	ar & ips.optional_settings_path;
	ar & ips.sensors_required;
	ar & ips.enable_image_enhancement;

	// TODO: Look into implementing serialization method for CUcontext.
	// ar & ips.sdk_cuda_ctx;
}


// ------------------------------- sl::InputType -------------------------------

template <class Archive>
void serialize(
	Archive& ar, 
	sl::InputType& it, 
	const unsigned int
	)
{
	// TODO: InputType doesn't seem to have any attributes, so it
	// might not be necessary to implement this!
}


// --------------------------------- sl::String --------------------------------

template <class Archive>
void save(Archive& ar, const sl::String& s, const unsigned int)
{
	// TODO: This implementation is ineffective. Look to improve
	// later!
	sl::String z = s; // Copy the sl::String.
	const char* ptr = z.get(); // Get pointer, not c-string!
	size_t size = z.size();
	std::string ss(ptr, size);
	ar & ss;
}

template <class Archive>
void load(Archive& ar, sl::String& s, const unsigned int)
{
	std::string ss;
	ar & ss;
	const char* ptr = ss.data();
	s.set(ptr);
}

// --------------------------- std::chrono::duration ---------------------------

template <class Archive, class Rep, class Period>
inline void serialize(
	Archive& ar, 
	std::chrono::duration<Rep, Period>& d,
	const unsigned int version
	)
{
	boost::serialization::split_free(ar, d, version);
}

template <class Archive, class Rep, class Period>
inline void save(
	Archive& ar,
	std::chrono::duration<Rep, Period> const& d,
	const unsigned int
	)
{
	ar << d.count();
}

template <class Archive, class Rep, class Period>
inline void load(
	Archive& ar,
	std::chrono::duration<Rep, Period>& d,
	const unsigned int
	)
{
	Rep rep;
	ar >> rep;
	d = std::chrono::duration<Rep, Period>(rep);
}

// ------------------------------- sl::Timestamp -------------------------------

template <class Archive>
inline void serialize(
	Archive& ar,
	sl::Timestamp& t,
	const unsigned int
	)
{
	ar & t.data_ns;
}

// --------------------------- sl::RuntimeParameters ---------------------------

template <class Archive>
inline void serialize(
	Archive& ar,
	sl::RuntimeParameters& rt,
	const unsigned int
	)
{
	// TODO: As of version 3.1 the sl::RuntimeParameters has an additional
	// member variable called texture_confidence_threshold. Look into adding
	// it to the serialization function.
	ar & rt.sensing_mode;
	ar & rt.measure3D_reference_frame;
	ar & rt.enable_depth;
	ar & rt.confidence_threshold;
	ar & rt.textureness_confidence_threshold;
}

// --------------------------- sl::RecordingParameters --------------------------

template <class Archive>
inline void serialize(
	Archive& ar,
	sl::RecordingParameters& rt,
	const unsigned int
	)
{
	ar & rt.video_filename;
	ar & rt.compression_mode;
}

}
};