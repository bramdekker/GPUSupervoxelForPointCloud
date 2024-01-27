#!/usr/bin/env bash

input_dir=""
k=""
seed_resolution=""
voxel_resolution=""

while getopts i:o:k:s:v: flag
do
    case "$flag" in
        i) input_dir="$OPTARG";;
        k) k="$OPTARG";;
        s) seed_resolution="$OPTARG";;
        v) voxel_resolution="$OPTARG";;
    esac
done

echo "Input directory is $input_dir"
echo "k is $k"
echo "seed is $seed_resolution"
echo "voxel is $voxel_resolution"

count=0
for fn in "$input_dir"/*.xyz; do
    bn=${fn##*/}
    # echo $bn
    output=$(./GPU-SS $fn $fn $seed_resolution $voxel_resolution 15 10 $k)
    
    IFS=' ' read -ra my_array <<< "$output"
    index=0
    for i in "${my_array[@]}"
    do            
        if [[ $index -eq 1 ]]; then  
            printf "$i\n" >> "gpu_ss_br.txt"
        elif [[ $index -eq 2 ]]; then
            printf "$i\n" >> "gpu_ss_use.txt"
        elif [[ $index -eq 3 ]]; then
            printf "$i\n" >> "gpu_ss_gce.txt"
        fi
        
        index=$((index+1))
    done
       
    # Output is: n_supervoxel br use gce n_supervoxel br use gce n_supervoxel br use gce
    # TODO: write this to files: bpss_br, bpss_use, bpss_gce, vcss_br etc
    # TODO: what to do about pcs with different number of supervoxels per method?
    
    count=$((count+1))
done

echo "Did the oversegmentation on ${count} boxes"
