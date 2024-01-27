#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#include <fstream>
#include <string>


static bool read_points_labels(const std::string& fileName, std::vector<double> &points, std::vector<int> &labels)
{
    int count = 0;
	points.clear();
	labels.clear();
	
	std::ifstream fileStream_all((char*)fileName.c_str());
	// cout << (char*)fileName.c_str() << endl;
	if (!fileStream_all)
	{
		cout << "load points failed!" << endl;
		return false;
	}
	
	
    std::string line;
    while (std::getline(fileStream_all, line)) {
        std::istringstream is(line);
	//while (!fileStream_all.eof())
	//{
	
		double x, y, z;
		int c;
		
		is >> x;
        is >> y;
        is >> z;
        is >> c;
        
		// fileStream_all >> x >> y >> z >> nx >> ny >> nz >> c;
		
		//if (count == 1) {
		    // cout << x << ", " << y << ", " << z << ", " << nx << ", " << ny << ", " << nz << ", " << c << endl; 
		//}

		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
		
		labels.push_back(c);
		
		count++;
	}
	fileStream_all.close();
	
	// cout << "Total number of points read from filestream is " << count << endl;
	return true;
}

static bool read_points_normals(const std::string& fileName, std::vector<double> &points, std::vector<double> &normals, std::vector<int> &labels)
{
    int count = 0;
	points.clear();
	normals.clear();
	labels.clear();
	
	std::ifstream fileStream_all((char*)fileName.c_str());
	// cout << (char*)fileName.c_str() << endl;
	if (!fileStream_all)
	{
		cout << "load points failed!" << endl;
		return false;
	}
	
	
    std::string line;
    while (std::getline(fileStream_all, line)) {
        std::istringstream is(line);
	//while (!fileStream_all.eof())
	//{
	
		double x, y, z, nx, ny, nz;
		int c;
		
		is >> x;
        is >> y;
        is >> z;
        is >> nx;
        is >> ny;
        is >> nz;
        is >> c;
        
		// fileStream_all >> x >> y >> z >> nx >> ny >> nz >> c;
		
		if (count == 1) {
		    // cout << x << ", " << y << ", " << z << ", " << nx << ", " << ny << ", " << nz << ", " << c << endl; 
		}

		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
		
		normals.push_back(nx);
		normals.push_back(ny);
		normals.push_back(nz);
		
		labels.push_back(c);
		
		count++;
	}
	fileStream_all.close();

	if (points.size() != normals.size())
	{
		cout << "size doesn't match" << endl;
		return false;
	}
	
	// cout << "Total number of points read from filestream is " << count << endl;
	return true;
}


static bool read_points_normals(std::string& posName, std::string& normalName, std::vector<double> &points, std::vector<double> &normals)
{
	points.clear();
	std::ifstream fileStream_pos((char*)posName.c_str());
	
	if (!fileStream_pos)
	{
		cout << "load points failed!" << endl;
		return false;
	}
		
	std::ifstream fileStream_normal((char*)normalName.c_str());
	if (!fileStream_normal)
	{
		cout << "load normal failed!" << endl;
		return false;
	}
		
	while (!fileStream_pos.eof())
	{
		double x, y, z;
		//char note;

		fileStream_pos >> x >> y >> z;


		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
	}
	fileStream_pos.close();

	while (!fileStream_normal.eof())
	{
		double x, y, z;
		//char note;

		fileStream_normal >> x >> y >> z;


		normals.push_back(x);
		normals.push_back(y);
		normals.push_back(z);
	}
	fileStream_normal.close();

	if (points.size() != normals.size())
	{
		cout << "size doesn't match" << endl;
		return false;
	}
	return true;
}


static void write_labels(const std::vector<int> &labels, std::string& fileName)
{
	std::ofstream labelresult(fileName);

	if (labelresult)
	{
		int vnb = (int)labels.size();
		for (int i = 0; i < vnb; ++i)
		{
			labelresult << labels[i];
			if (i < vnb - 1)
				labelresult << std::endl;
		}
	}

	labelresult.close();
}


#endif
