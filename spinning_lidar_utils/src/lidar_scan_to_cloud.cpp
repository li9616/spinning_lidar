
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf/transform_listener.h>
#include <laser_geometry/laser_geometry.h>


ros::Publisher laser_cloud_pub;
std::string target_frame, laser_scan_topic, laser_cloud_topic;
tf::TransformListener* tf_listener_ptr;
laser_geometry::LaserProjection laser_projector;

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
  // Projection of laser scans into point clouds
  sensor_msgs::PointCloud2 cloud;
  try
  {
    laser_projector.transformLaserScanToPointCloud(target_frame, *scan, cloud, *tf_listener_ptr);
  }
  catch (tf::TransformException& e)
  {
    // std::cout << e.what();
    return;
  }

  // cloud.header = scan->header;
  cloud.header.stamp = ros::Time::now();
  cloud.header.frame_id = target_frame;
  laser_cloud_pub.publish(cloud);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "lidar_scan_to_cloud");
  ros::NodeHandle nh;
  ros::NodeHandle priv_nh("~");

  priv_nh.param("target_frame", target_frame, std::string("base_link"));
  priv_nh.param("laser_scan_topic", laser_scan_topic, std::string("/front/scan"));
  priv_nh.param("laser_cloud_topic", laser_cloud_topic, std::string("front/cloud"));
  tf_listener_ptr = new tf::TransformListener();

  ROS_INFO("Transforming laser scans on '%s' to PointCloud2 messages, published as '%s'", laser_scan_topic.c_str(), laser_cloud_topic.c_str());
  laser_cloud_pub = nh.advertise<sensor_msgs::PointCloud2>(laser_cloud_topic, 1);
  ros::Subscriber laser_scan_sub = nh.subscribe(laser_scan_topic, 1, scanCallback);
    
  ros::spin();

  return EXIT_SUCCESS;
}
