#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>
#include <vector>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


cl_program loadProgram(cl_context context, const char* filename)
{
        std::ifstream in(filename, std::ios_base::binary);
        if(!in.good()) {
                return 0;
        }

        // get file length
        in.seekg(0, std::ios_base::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios_base::beg);

        // read program source
        std::vector<char> data(length + 1);
        in.read(&data[0], length);
        data[length] = 0;

        // create and build program
        const char* source = &data[0];
        cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
        if(program == 0) {
                return 0;
        }

        if(clBuildProgram(program, 0, 0, 0, 0, 0) != CL_SUCCESS) {
                return 0;
        }

        return program;
}

int main(int argc, char const *argv[])
{

    unsigned int i=0, a, input_size;
    std::fstream inFile("input", std::ios_base::in);
    std::ofstream outFile("0556087.out", std::ios_base::out);

    inFile >> input_size;

    
    unsigned int *image = new unsigned int[input_size];
    
    while( inFile >> a ) {
        image[i++] = a;
    }
    
    
    cl_int err;
    cl_uint num;
    err = clGetPlatformIDs(0, 0, &num);
    if(err != CL_SUCCESS) {
        std::cerr << "Unable to get platforms\n";
        return 0;
    }

    std::vector<cl_platform_id> platforms(num);
    err = clGetPlatformIDs(num, &platforms[0], &num);
    if(err != CL_SUCCESS) {
        std::cerr << "Unable to get platform ID\n";
        return 0;
    }

    cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0 };
    cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, NULL);
    if(context == 0) {
        std::cerr << "Can't create OpenCL context\n";
        return 0;
    }

    size_t cb;
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    std::vector<cl_device_id> devices(cb / sizeof(cl_device_id));
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, &devices[0], 0);

    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    std::string devname;
    devname.resize(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, &devname[0], 0);
    std::cout << "Device: " << devname.c_str() << "\n";

    cl_command_queue queue = clCreateCommandQueue(context, devices[0], 0, 0);
    if(queue == 0) {
        std::cerr << "Cant create command queue\n";
        clReleaseContext(context);
        return 0;
    }

    cl_mem image_rgb = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * input_size, &image[0], NULL);
    cl_mem historgram_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * 256 * 3, NULL, NULL);

    if(image_rgb == 0 || historgram_res == 0) {
        std::cerr << "Can't create OpenCL buffer\n";
        clReleaseMemObject(image_rgb);
        clReleaseMemObject(historgram_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    cl_program program = loadProgram(context, "histogram.cl");
    if(program == 0) {
        std::cerr << "Can't load or build program\n";
        clReleaseMemObject(image_rgb);
        clReleaseMemObject(historgram_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);

        return 0;
    }

    cl_kernel histogram = clCreateKernel(program, "histogram", 0);
    if(histogram == 0) {
        std::cerr << "Can't load kernel\n";
        clReleaseProgram(program);
        clReleaseMemObject(image_rgb);
        clReleaseMemObject(historgram_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    clSetKernelArg(histogram, 0, sizeof(cl_mem), &image_rgb);
    clSetKernelArg(histogram, 1, sizeof(cl_mem), &historgram_res);
    clSetKernelArg(histogram, 2, sizeof(unsigned int), &input_size);

    size_t work_size = 256 * 3;
    std::vector<unsigned int> res(256 * 3);
    err = clEnqueueNDRangeKernel(queue, histogram, 1, 0, &work_size, 0, 0, 0, 0);
    if(err == CL_SUCCESS) {
        err = clEnqueueReadBuffer(queue, historgram_res, CL_TRUE, 0, sizeof(unsigned int) * 256 * 3, &res[0], 0, 0, 0);
    if(err == CL_SUCCESS) {
                for(int i = 0; i < 256 * 3; i++) {
                        if(i % 256 == 0 && i != 0)
                                outFile << std::endl;
                        outFile << res[i] << ' ';
                }
        } else {
                std::cerr << "Enqueue read buffer command error! \n";
        }

    } else {
        std::cerr << "Enqueue kernel command error! \n";
    }

    clReleaseMemObject(image_rgb);
    clReleaseMemObject(historgram_res);
    clReleaseProgram(program);
    clReleaseKernel(histogram);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    

    inFile.close();
    outFile.close();

    return 0;
}