#include "GPUSupervoxel.h"
#include "pc_io.h"

/*
Suggested Parameter Setting:
Okaland: voxel resolution R_v = 0.2, supervoxel resolution R_s =[0.4-1.8], lambda1=0.1,lambda2=0.02;
Semantic3D: voxel resolution R_v = 0.1, supervoxel resolution R_s =[1-1.8], lambda1=1,lambda2=0.2;
NYUV2: voxel resolution R_v = 0.01, supervoxel resolution R_s =[0.1-0.25], lambda1=0.01,lambda2=0.1/R_s;Swapping lambda1= lambda1*2, lambda2= lambda2*2

*/



//string get_imgname(string filename_txt)
//{
//	string::size_type pos, _pos2;
//
//	pos = filename_txt.find('.');
//	while (string::npos != pos)
//	{
//		_pos2 = pos + 1;
//		pos = filename_txt.find('.', _pos2);
//	}
//	_pos2 -= 1;
//	return filename_txt.substr(0, _pos2);
//}


//void getFiles(string path, vector<string>& files, vector<string>& file_names)
//{
//	//long hFile = 0;//win32
//	intptr_t hFile = 0;//x64
//	struct _finddata_t fileinfo;
//	string p;
//	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
//	{
//		do
//		{
//			if (!(fileinfo.attrib & _A_SUBDIR))
//			{
//				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
//				file_names.push_back(fileinfo.name);
//				//cout << fileinfo.name << endl;
//			}
//		} while (_findnext(hFile, &fileinfo) == 0);
//		_findclose(hFile);
//	}
//	string image_name;
//	for (unsigned int i = 0; i < files.size(); ++i)
//	{
//		image_name = get_imgname(file_names[i]);
//		file_names[i] = image_name;
//	}
//}




int main(int argc, char **argv)
{
	std::vector<double> points;
	std::vector<double> normals;
	std::vector<int> gt_point_labels;
	
	
    const std::string point_cloud_file_name = argv[1];
    const std::string save_name = argv[2];
    
    const double seedResolution = std::stod(argv[3]);
    const double voxelResolution = std::stod(argv[4]);
	
	const int max_cluster_iter = std::stoi(argv[5]);
	const int max_swap_iter = std::stoi(argv[6]);
	const int k_neighbours = std::stoi(argv[7]);
	
	//double voxelResolution = 0.2;
	//double seedResolution = 1.2;
	points.clear();
	normals.clear();
	gt_point_labels.clear();
	
	//specify your save path
	// string save_name = "../../example/4.txt";
	/*
	save_type
	0:labels of point cloud;
	1:pseudo-color supervoxels of point cloud, open the .txt in CloudCompare software
	*/
	int save_type = 1;
	//Provide the paths of coordinates and normal of the point cloud
	std::string points_name = "../../example/1.pcd.xyz";
	std::string normals_name = "../../example/1.pcd_PCA.norm";
	clock_t startTime, endTime;
	startTime = clock();
	//cout << "Just before reading points" << endl;
	bool read = read_points_labels(point_cloud_file_name, points, gt_point_labels);
	//bool read = read_points_normals(points_name, normals_name, points, normals);
	endTime = clock();
	//cout << "loading data to CPU time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	if (read)
	{
	    //cout << "The number of points read is " << (int)points.size() / 3 << endl;
		GPUSupervoxel pc_supervoxel;
		double normal_factor = 0.75; // lambda1 in distance metric
		double spatial_factor = 0.5; // lambda2 in distance metric
		double lambda1 = 1, lambda2 = 0.2;
		pc_supervoxel.JFASupervoxel(points, normals, gt_point_labels, voxelResolution, seedResolution, normal_factor, spatial_factor, save_name, save_type, max_cluster_iter, max_swap_iter, k_neighbours);
	}

	return 0;
}
