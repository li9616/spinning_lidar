
#ifndef LOOP_CLOSURE_HANDLER_H
#define LOOP_CLOSURE_HANDLER_H

#include <ros/ros.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/ColorRGBA.h>
#include <visualization_msgs/Marker.h>
#include <tf/transform_broadcaster.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include "g2o/core/sparse_optimizer.h"
#include "g2o/types/icp/types_icp.h"
#include "g2o/types/slam3d/types_slam3d.h"

#include "utils/pose6DOF.h"
#include "utils/geometric_utils.h"

class PoseOptimizerG2O
{
private:
    typedef ros::Time TimeStamp;
    typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

    int verbosity_level_;

    uint curr_vertex_key_, curr_edge_key_;
    std::map<uint, PointCloud::Ptr> graph_scans_;
    std::map<uint, Pose6DOF> graph_poses_;
    std::map<uint, std::pair<uint, uint>> graph_edges_;
    g2o::SparseOptimizer* optimizer_;
    Pose6DOF latest_pose;
    int pose_opt_iters;

    ros::NodeHandle nh_;
    tf::TransformListener tf_listener_;
    tf::TransformBroadcaster tf_broadcaster_;
    ros::Timer map_transform_timer_;
    std::string namespace_, graph_edges_topic_, graph_vertices_topic_, graph_keyframes_topic_, increment_cloud_topic_;
    std::string laser_frame_, robot_frame_, odom_frame_, map_frame_;
    ros::Publisher graph_edges_pub_, graph_vertices_pub_, graph_keyframes_pub_, increment_cloud_pub_;
    bool publish_map_transform_;
    

    std_msgs::ColorRGBA vertex_color_;
    std_msgs::ColorRGBA odom_edge_color_;
    std_msgs::ColorRGBA closure_edge_color_;
    std_msgs::ColorRGBA keyframes_color_;
    geometry_msgs::Vector3 keyframes_scale_;
    geometry_msgs::Vector3 vertex_scale_;
    geometry_msgs::Vector3 closure_edge_scale_;
    geometry_msgs::Vector3 odom_edge_scale_;


public:

    PoseOptimizerG2O(ros::NodeHandle nh);

    void init();

    void setGraphMarkersProperties();

    void loadParameters();

    void advertisePublishers();

    void addNewKeyframeVertex(PointCloud::Ptr *new_cloud_ptr, Pose6DOF icp_transform, Pose6DOF pose, uint *key);

    void addNewOdometryVertex(PointCloud::Ptr *new_cloud_ptr, Pose6DOF pose, uint *key);

    void addNewEdge(Eigen::MatrixXd cov, uint vertex2_key, uint vertex1_key, uint *key);

    bool optimizeGraph();

    void refinePoseGraph();

    void refineVertices();

    void refineEdges();

    Pose6DOF getStartPose();

    Pose6DOF getLatestPose();

    bool checkLoopClosure();

    void publishPoseGraphMarkers();

    void mapTransformCallback(const ros::TimerEvent&);

    void gazeboMapTransformCallback(const ros::TimerEvent&);

    void publishRefinedMap();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#endif