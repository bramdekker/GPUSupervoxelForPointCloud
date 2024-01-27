#pragma once
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include "Texture.h"
#include <array.h>
#include <point_3d.h>
class GPUSupervoxel
{
protected:
	struct Voxel
	{
		int index;//valid index of voxel
		int id;//3d index, based on width,height,depth
		int i, j, k;
		double centroid[3];
		double normal[3];
		std::vector<int> members;
		int grid;//store grid map of voxels
		Voxel()
			: index(-1), id(-1), grid(-1), i(-1), j(-1), k(-1)
		{
			members.clear();
		}
	};

	struct Supervoxel
	{
		int index;//valid index of supervoxel
		int id;//3d index, based on superWidth,superHeight,superDepth
		const Voxel *centroid;
		Supervoxel()
			: index(-1), centroid(NULL)
		{}
	};



	double normalWeight;
	double spatialWeight;
	double voxelResolution;
	double seedResolution;

	std::vector<Voxel> voxels;
	std::unordered_map<int, int> voxelMap;
	std::vector<Supervoxel> supervoxels;


	int point_num;//number of 3d points
	int width, height, depth;
	int width_t, height_t;
	int superWidth, superHeight, superDepth;
	int s;
	int seeds_num;//number of seeds
	double bbox[6];
	std::vector<float> data_3d;
	std::vector<float> normal_3d;
	std::vector<float> seeds_t;
	std::unordered_map<int, int> supervoxelMap;//index,id correspondence
	vector<float> voxel_gridId;
	vector<float> seed_neighbor_map;
	vector<float> seed_neighbors;
	vector<float> seed_valid;
	vector<float> seed_cov_info;
	Texture texture;
	int min_size;
	string save_path;
	string save_name;

	int* labels;

	void voxelization(const std::vector<double> &points,
		const std::vector<double> &normals, double voxelResolution_, double seedResolution_);
	void nearest_valid_seed_voxel(std::vector<float>& seeds_3d, int Lloyd_curr_iter);
	void update_seeds_neighbors();
	void update_seeds_neighbors_swap();
	void generate_seg(const string& save_path, int* labels, const std::vector<double> &points,
		const std::vector<double> &normals, int* render_color);
	void generate_label(const string& save_path, int* labels);

public:
	GPUSupervoxel();
	~GPUSupervoxel();

	void GetSupervoxelLabels(int* labels, cl::Array<int> *s_labels);
	void GetNormalsVector(std::vector<double> &normals, cl::Array<cl::RVector3D> &normals_arr);
	cl::Array<cl::RPoint3D> GetPointsArray(const std::vector<double> &points);
	cl::Array<int> GetGtLabelsArray(const std::vector<int> &gt_labels);
	cl::Array<cl::Array<int>> GetNeighbors(cl::Array<cl::RPoint3D> &points);
    	
	int JFASupervoxel(const std::vector<double> &points, std::vector<double> &normals, const std::vector<int> &gt_labels, double voxelResolution_, double seedResolution_, double lambda1, double lambda2, const string& save_name, int save_type, const int max_cluster_iter, const int max_swap_iter, const int k);

	void print_data(vector<float>& data_3d, string& txt_name, int bit);
	void print_valid(vector<int>& valid_flag);
};

