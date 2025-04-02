import vitis
import os

part = 'xc7z020clg400-1'
clock = "10"

# cwd = os.getcwd()+'/'
cwd = "/workspaces/HLSLinearAlgebra/src/"
workspace_path = cwd + "vitis_workspace/"

# Initialize session
client = vitis.create_client()
client.set_workspace(path=workspace_path)

component_name = "floatX"

component_path = workspace_path + component_name + "/"

# Delete the component if it already exists
if os.path.exists(component_path):
    client.delete_component(name=component_name)
    # continue

# Create component.
# Create new config file in the component folder
# of the workspace
comp = client.create_hls_component(name=component_name,
                                    cfg_file=['hls_config.cfg'],
                                    template='empty_hls_component')

# Get handle of config file, then programmatically set desired options
cfg_file = client.get_config_file(path=component_path + "hls_config.cfg")
cfg_file.set_value(key='part', value=part) 
cfg_file.set_values(section='hls', key='syn.file',
                    values=[cwd+'floatX.h'])
#cfg_file.set_value(section='hls', key='syn.file_cflags',
#                    value=cwd+(f"conv2D_3x3_IM.cpp, "
#                                f"-DTOP_NAME={component_name} "
#                                f"-DUSE_RELU={use_relu} "
#                                f"-DW_DATA_TYPE={w_data_type} "
#                                f"-DA_DATA_TYPE={a_data_type} "
#                                f"-DBATCH_SIZE={batch_size} "
#                                f"-DIN_CHANNELS={in_channels} "
#                                f"-DOUT_CHANNELS={out_channels} "
#                                f"-DIN_HEIGHT={in_height} "
#                                f"-DIN_WIDTH={in_width} "
#                                f"-DPADDING={padding}"))
cfg_file.set_values(section='hls', key='tb.file',
                    values=[cwd+'floatX_test.cpp'])
#cfg_file.set_value(section='hls', key='tb.file_cflags',
#                    value=cwd+(f"conv2D_3x3_IM_test.cpp, "
#                                f"-DTOP_NAME={component_name} "
#                                f"-DUSE_RELU={use_relu} "
#                                f"-DW_DATA_TYPE={w_data_type} "
#                                f"-DA_DATA_TYPE={a_data_type} "
#                                f"-DBATCH_SIZE={batch_size} "
#                                f"-DIN_CHANNELS={in_channels} "
#                                f"-DOUT_CHANNELS={out_channels} "
#                                f"-DIN_HEIGHT={in_height} "
#                                f"-DIN_WIDTH={in_width} "
#                                f"-DPADDING={padding} "
#                                "-I/usr/include/opencv4 "
#                                "-lopencv_core "
#                                "-lopencv_highgui "
#                                "-lopencv_imgcodecs"))
#cfg_file.set_value(section='hls', key='syn.top',
#                    value=component_name)
cfg_file.set_value(section='hls', key='clock', value=clock)
cfg_file.set_value(section='hls', key='flow_target',
                    value='vivado')
# cfg_file.set_value (section = 'hls',
# key = 'package.output.syn',    value = '0')
# cfg_file.set_value (section = 'hls',
# key = 'package.output.format', value = 'rtl')
# cfg_file.set_value (section = 'hls',
# key = 'package.output.format', value = 'ip_catalog')
# cfg_file.set_value (section = 'hls',
# key = 'package.ip.display_name',   value = component_name)
# cfg_file.set_value (section = 'hls',
# key = 'package.ip.name',   value = component_name)
# cfg_file.set_value (section = 'hls',
# key = 'csim.code_analyzer',    value = '0')

# cfg_file.set_value (section = 'hls',
# key = 'syn.compile.pipeline_style',    value = 'frp')
# cfg_file.set_value (section = 'hls',
# key = 'syn.dataflow.default_channel',    value = 'fifo')
# cfg_file.set_value (section = 'hls',
# key = 'syn.dataflow.fifo_depth',    value = '16')
# cfg_file.set_value (section = 'hls',
# key = 'syn.directive.interface',    value = 'free_pipe_mult mode=ap_fifo B')
# cfg_file.set_value (section = 'hls',
# key = 'syn.directive.interface',    value = 'free_pipe_mult mode=ap_fifo out')

# Run flow steps
comp = client.get_component(name=component_name)
comp.execute(operation='C_SIMULATION')
# comp.execute(operation='SYNTHESIS')
# comp.execute(operation='CO_SIMULATION')
# comp.execute(operation='IMPLEMENTATION')
