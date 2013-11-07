VTK_DIR=/home/labasan1/visit/vtk/5.8.0.a/linux-x86_64_gcc-4.4/
VTK_LIBS=-lvtkGraphics -lvtkCommon -lvtksys -lvtkFiltering -lvtkverdict
MP_DIR=/opt/mvapich2-gnu-psm-1.7/

# Object file builds with libmsr
vtk_power: vtk_power.C
	mpicc  -o vtk_power -I$(VTK_DIR)/include -L$(VTK_DIR)/lib $(VTK_LIBS) -I$(MP_DIR)/include -L/home/labasan1/libmsr -lmsr -L/home/labasan1/libmsr -lPowThermTest vtk_power.C

# Object file builds normally without power reads (i.e. libmsr). Purpose was to determine which isovalue had the largest runtime.
vtk_app: vtk_power.C
	mpicc -o vtk_app -I$(VTK_DIR)/include -L$(VTK_DIR)/lib $(VTK_LIBS) -I$(MP_DIR)/include vtk_power.C

clean:
	rm vtk_power vtk_app
