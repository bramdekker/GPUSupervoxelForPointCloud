#ifndef GEOMETRY_UTIL_EVALUATE_H_
#define GEOMETRY_UTIL_EVALUATE_H_

#include <map>

#include <array.h>
#include <log.h>
#include <point_3d.h>
#include <distance_3d.h>

namespace cl {
namespace geometry {
namespace util {

bool HasDifferentLabel(const int& label, const Array<int>& labels, const Array<int>& neighbors) {
    // assert(neighbors.size() == 15); not true for variable k!
    for (int i = 0; i < neighbors.size(); i++) {
        if (labels[neighbors[i]] != label) {
            return true;
        }
    }
    return false;
}

// Check if ground truth boundary point p has a matching supervoxel boundary point.
bool HasMatchingBoundaryPoint(RPoint3D p, int idx, const Array<RPoint3D>& points, const Array <int>& s_boundaries, const double e) {
    for (int i = 0; i < s_boundaries.size(); i++) {
        if (Distance(p, points[s_boundaries[i]]) < e) {
            return true;
        }
    }
    return false;
} 

/**
 * Get the boundary points of an array.
 */
void GetBoundaryPoints(const Array<int>& labels,
                        const Array<Array<int> >& neighbors,
                        Array<int>* boundaries) {
    for (int i = 0; i < labels.size(); i++) {
        if (HasDifferentLabel(labels[i], labels, neighbors[i])) {
            boundaries->emplace_back(i);
        }
    }
    
    return;
}

void print_array(Array<int>& arr)
{
  LOG(INFO) << "{";
  for (int i = 0; i < arr.size() - 1; i++)
  {
    LOG(INFO) << arr[i] << ", ";
  }
  LOG(INFO) << arr[arr.size()-1] << "}\n";
}

Array<int> GetDistinctLabels(const Array<int>& labels) {
    Array<int> distinct_labels;
    
    for (int i = 0; i < labels.size(); i++) {
        if (!distinct_labels.contains(labels[i])) {
            distinct_labels.push_back(labels[i]);
        }
    }
    
    return distinct_labels;
}

Array<Array<int>> GetGtSegments(const Array<int>& gt_labels) {
    // Get all different labels 
    Array<int> distinct_labels = GetDistinctLabels(gt_labels);
    // LOG(INFO) << "The distinct labels for this data sample are: ";
    //for (int i = 0; i < distinct_labels.size(); i++) {
    //    LOG(INFO) << distinct_labels[i];
    //}
    
    Array<Array<int>> gt_segments(distinct_labels.size());
                    
    // For all labels 
    for (int i = 0; i < gt_labels.size(); i++) {
        gt_segments[distinct_labels.index(gt_labels[i])].push_back(i);
    }          
    
    assert(gt_segments.size() == distinct_labels.size());              
    
    return gt_segments;
}

Array<int> GetOverlappingSupervoxels(Array<int> gt_segment, const Array<int>& s_labels) {
    Array<int> overlapping_labels;
    
    for (int i = 0; i < gt_segment.size(); i++) {
        // point_idx = gt_segment[i]
        // s_labels holds supervoxel label for each point
        if (!overlapping_labels.contains(s_labels[gt_segment[i]])) {
            overlapping_labels.push_back(s_labels[gt_segment[i]]);
        }
    }
    
    return overlapping_labels;
}

int GetSupervoxelPointCount(Array<int> overlapping_supervoxels, const Array<int>& s_labels) {
    int count = 0;
    
    for (int i = 0; i < overlapping_supervoxels.size(); i++) {
        int supervoxel_label = overlapping_supervoxels[i];
        count += s_labels.count(supervoxel_label);
    }
    
    return count;
}

int GetNumberOfOverlappingPoints(Array<int> point_indices, Array<int> s_labels, int supervoxel_label) {
    int n = 0;
    
    // For indices in point_indices
    // Check if supervoxel label at that index is supervoxel_label
    
    for (int i = 0; i < point_indices.size(); i++) {
        if (s_labels[point_indices[i]] == supervoxel_label) {
            n++;
        }
    }
    
    return n;
}

/**
 * Calculate boundary recall and under-segmentation metrics.
 */
void PrintMetrics(const Array<RPoint3D>& points,
                    const Array<int>& gt_labels,
                    const Array<int>& s_labels,
                    const Array<Array<int> >& neighbors,
                    const int k, const double e, int n_supervoxels) {
    assert(gt_labels.size() == s_labels.size());

    // Get gt boundaries
    Array<int> gt_boundaries; // indices of points
    GetBoundaryPoints(gt_labels, neighbors, &gt_boundaries);
    // LOG(INFO) << "There are " << gt_boundaries.size() << " ground truth boundary points.";
    //cout << "There are " << gt_boundaries.size() << " ground truth boundary points." << endl;
    
    // Get s boundaries
    Array<int> s_boundaries;
    GetBoundaryPoints(s_labels, neighbors, &s_boundaries);
    // LOG(INFO) << "There are " << s_boundaries.size() << " supervoxel boundary points.";
    //cout << "There are " << s_boundaries.size() << " supervoxel boundary points." << endl;
    
    // Loop over all gt bounadries points and check if there exists a s boundary point with less than e distance
    // Divide by total number of boundary points in gt
    // For all points that are gt boundary:
    //      Check if there exists a point in the supervoxel boundaries, for which the distance towards it is less then e
    //cout << "Points array has a size of " << points.size() << endl;
    int count = 0;
    for (int i = 0; i < gt_boundaries.size(); i++) {
        if (HasMatchingBoundaryPoint(points[gt_boundaries[i]], i, points, s_boundaries, e)) {
            count++;
        }
        
    //    if (count > 0 && count % 1000 == 0) {
    //        LOG(INFO) << count << " matching boundary points are found!";
    //    }
    }
    
    //cout << "Found " << count << " matching boundary points." << endl;
    
    double br;
    if (gt_boundaries.size() == 0) {
        br = 1.0;
    } else {
        br = (double) count / gt_boundaries.size();
    }
    
    // LOG(INFO) << "The number of matched boundary points is " << count;
    cout << std::setprecision(5) << br << " ";
    
    // Get total number of points
    int N = gt_labels.size();
    
    // Get gt_segments (14 in this case, 1 for each label)
    Array<Array<int>> gt_segments = GetGtSegments(gt_labels);
    // LOG(INFO) << "Just after the GetGtSegments() call for ground truth";
    
    assert(gt_segments.size() > 0);
    
    //for (int j = 0; j < gt_segments.size(); j++) {
        // LOG(INFO) << "Gt segment " << j << " contains " << gt_segments[j].size() << " points";
    //}
    
    // For all gt segments
    // - Get a list with all labels of supervoxels that are overlapping
    //   - For all overlapping supervoxels, get the number of points of the whole supervoxel
    
    int point_sum = 0;
    for (int i = 0; i < gt_segments.size(); i++) {
        Array<int> overlapping_supervoxels = GetOverlappingSupervoxels(gt_segments[i], s_labels);
        // overlapping_supervoxels == supervoxel labels 
        
        // LOG(INFO) << "Gt segment " << i << " has " << overlapping_supervoxels.size() << " overlapping supervoxels.";
        
        //for (int j = 0; j < overlapping_supervoxels.size(); j++) {
        //    LOG(INFO) << "Overlapping supervoxel " << j << " has label " << overlapping_supervoxels[j];
        //}
        
        for (int j = 0; j < overlapping_supervoxels.size(); j++) {
            int cur_count = s_labels.count(overlapping_supervoxels[j]);
            //LOG(INFO) << "Supervoxel with label " << supervoxel_label << " contains " << cur_count << " points.";
            point_sum += cur_count;
        }
    }
    
    // LOG(INFO) << "The total sum of all points in overlapping supervoxels is " << point_sum;
    
    // Get supervoxels segments
    // Supervoxels are already segments on their own! in s_labels: different label is segment
    //Array<Array<int>> s_segments;
    //GetSegments(s_labels, neighbors, &s_segments);
    
    //assert(s_segments.size() > 0);
    //assert(s_segments.size() == n_supervoxels);
    //LOG(INFO) << "The number of segments in the supervoxels is " << s_segments.size();
    
    // int sp_points = 0;
    // for (int i = 0; i < gt_segments.size(); i++) {
    //     Array<Array<int>> overlapping_segments = GetOverlappingSegments(gt_segments[i], s_segments);
        
    //     for (int j = 0; j < overlapping_segments.size(); j++) {
    //         sp_points += overlapping_segments[j].size();
    //     }
    // }
    
    double use = (point_sum - N) / (float) N;
    cout << use << " ";
    
    double p = 0.0, q = 0.0;
    int total_intersection = 0;
    
    for (int i = 0; i < gt_segments.size(); i++) {
        // Get number of points in gt segment
        int gt_points = gt_segments[i].size();
        // LOG(INFO) << "Number of points in ground truth segment is " << gt_points;
        
        Array<int> overlapping_supervoxels = GetOverlappingSupervoxels(gt_segments[i], s_labels);
            
        for (int j = 0; j < n_supervoxels; j++) {
            if (!overlapping_supervoxels.contains(j)) { // Not overlapping -> 0
                continue;    
            }
            
            // j == supervoxel label
            int overlapping_points = GetNumberOfOverlappingPoints(gt_segments[i], s_labels, j);
            // LOG(INFO) << "Number of overlapping points is " << overlapping_points;
    
            // Get number of points in supervoxel
            int s_points = s_labels.count(j);
            // LOG(INFO) << "Number of points in supervoxel is " << s_points;
            
            total_intersection += overlapping_points;
            p += (1 - (overlapping_points / gt_points)) * overlapping_points;
            q += (1 - (overlapping_points / s_points)) * overlapping_points;      
        }
    }
    // For all gt_segments:
    // - For all supervoxels:
    //   - Get number of overlapping points
    
    // For all supervoxels:
    // - For all gt segments:
    //   - Get number of overlapping points
    
    double gce = std::min(p, q) / total_intersection;
    cout << gce << " ";
        
    return;
}

} // namespace util
} // namespace geometry
} // namespace cl

#endif // GEOMETRY_UTIL_EVALUATE_H_
