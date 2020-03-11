#include <ZEDutils/ZEDio.hpp>
#include <ZEDutils/ZEDnetwork.hpp>
#include <ZEDutils/ZEDtypes.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sl/Camera.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
	// Initialize mutex, camera and hive
	//boost::shared_ptr<boost::mutex> mutex_ptr(new boost::mutex());
	sl::Camera* cam = new sl::Camera();
	boost::shared_ptr<Hive> hive(new Hive(boost::shared_ptr<boost::mutex>(
		new boost::mutex())));

	
	// Open camera
	sl::InitParameters init_params;
	init_params.sdk_verbose = true;
	init_params.camera_fps = 30;
	init_params.camera_resolution = sl::RESOLUTION::HD720;
	auto error = cam->open(init_params);
	if(error != sl::ERROR_CODE::SUCCESS)
	{
		std::cout << "[ERROR] " << sl::toVerbose(error) << std::endl;
	}
	
		
	// Enable recording
	sl::String rec_file = "/home/martin/dev/zedutils/data/server_test.svo";
	auto rec_compress = sl::SVO_COMPRESSION_MODE::H264;
	auto rec_params = sl::RecordingParameters(rec_file, rec_compress);
	error = cam->enableRecording(rec_params);
	if(error != sl::ERROR_CODE::SUCCESS)
	{
		std::cout << "[ERROR] " << sl::toVerbose(error) << std::endl;
	}
	
	
	// Set up acceptors and connections
	boost::shared_ptr<ZEDAcceptor> l_accptr(new ZEDAcceptor(hive));
	boost::shared_ptr<ImageConnection> l_conn(new ImageConnection(
		hive));
	l_accptr->Listen("127.0.0.1", 10000);
	l_accptr->Accept(l_conn);
	boost::shared_ptr<ZEDAcceptor> r_accptr(new ZEDAcceptor(hive));
	boost::shared_ptr<ImageConnection> r_conn(new ImageConnection(
		hive));
	r_accptr->Listen("127.0.0.1", 11000);
	r_accptr->Accept(r_conn);
	
	
	// Initialize images and byte vectors
	sl::Mat l_img, r_img;
	std::vector<boost::uint8_t> l_vec, r_vec;
	
	
	// Record loop
	unsigned int frame_cnt = 0;
	unsigned int frame_tot = 3000;
	unsigned int send_every = 10;
	try
	{
		while(frame_cnt < frame_tot) // While no interrupt or quit
		{
			error = cam->grab();
			if(error == sl::ERROR_CODE::SUCCESS)
			{
				frame_cnt++;
				std::cout << "Frames: " << frame_cnt << "/"
					<< frame_tot << std::endl;
				
				// Retrieve images
				cam->retrieveImage(l_img, sl::VIEW::LEFT);
				cam->retrieveImage(r_img, sl::VIEW::RIGHT);
				
				// If send frame
				if(frame_cnt % send_every == 0)
				{
					// TODO: Authenticate clients before
					// sending
					// Create byte vectors
					l_vec = slMat2Vec(l_img);
					r_vec = slMat2Vec(r_img);
					
					// Collect threads
					hive->Poll();
					
					// Send byte vectors
					l_conn->Send(l_vec);
					r_conn->Send(r_vec);
					
					// Sleep
					boost::this_thread::sleep(
						boost::posix_time::milliseconds(
						1));
				}
			}
			else
			{
				std::cout << "[ERROR] " << sl::toVerbose(error) 
					<< std::endl;
			}
		}
	}
	catch(std::exception& ex)
	{
		std::cout << "[EXCEPTION]: " << ex.what() << std::endl;
	}
	
	
	// Stop hive
	hive->Stop();
	
	
	// Close and deallocate camera
	cam->close();
	delete cam;
	
	return 0;
}
