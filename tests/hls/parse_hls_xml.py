import os
import xml.etree.ElementTree as ET


def parse_xml_reports(folder):
    """
    Parse Vivado/Vitis HLS *.xml file to extract resource usage and latency.
    Returns a dictionary of results.
    """

    csynth_path = folder + "/hls/syn/report/csynth.xml"
    impl_path = folder + "/hls/impl/report/verilog/export_impl.xml"

    if os.path.exists(csynth_path):
        csynt_results = parse_csynt_reports(csynth_path)
    else:
        csynt_results = {}
    
    if os.path.exists(impl_path):
        impl_results = parse_impl_reports(impl_path)
    else:
        impl_results = {}
        
    return csynt_results, impl_results


def parse_csynt_reports(xml_file):

    tree = ET.parse(xml_file)
    root = tree.getroot()  # <profile> is typically the root

    results = {}

    # PerformanceEstimates -> SummaryOfOverallLatency
    perf_node = root.find('PerformanceEstimates')
    if perf_node is not None:
        latency_node = perf_node.find('SummaryOfOverallLatency')
        if latency_node is not None:
            # results['Latency_best'] = latency_node.attrib.get('Best-caseLatency')
            # results['Latency_worst'] = latency_node.attrib.get('Worst-caseLatency')
            # results['latency'] = latency_node.find('Best-caseLatency').text
            results['latency'] = int(latency_node.find('Worst-caseLatency').text)
        latency_node = perf_node.find('SummaryOfTimingAnalysis')
        if latency_node is not None:
            results['clock'] = float(latency_node.find('EstimatedClockPeriod').text)

    # AreaEstimates -> Resources
    area_node = root.find('AreaEstimates')
    if area_node is not None:
        res_node = area_node.find('Resources')
        if res_node is not None:
            # Example: <BRAM_18K>2</BRAM_18K>, <DSP48E>8</DSP48E> ...
            results['BRAM'] = int(res_node.find('BRAM_18K').text)
            results['DSP'] = int(res_node.find('DSP').text)
            results['FF'] = int(res_node.find('FF').text)
            results['LUT'] = int(res_node.find('LUT').text)

    return results


def parse_impl_reports(xml_file):
    """
    Parse Vivado/Vitis HLS *.xml file to extract resource usage and latency.
    Returns a dictionary of results.
    """
    tree = ET.parse(xml_file)
    root = tree.getroot()  # <profile> is typically the root

    results = {}

    # PerformanceEstimates -> SummaryOfOverallLatency
    perf_node = root.find('TimingReport')
    if perf_node is not None:
        results['clock'] = float(perf_node.find('AchievedClockPeriod').text)

    # AreaEstimates -> Resources
    area_node = root.find('AreaReport')
    if area_node is not None:
        res_node = area_node.find('Resources')
        if res_node is not None:
            # Example: <BRAM_18K>2</BRAM_18K>, <DSP48E>8</DSP48E> ...
            results['BRAM'] = int(res_node.find('BRAM').text)
            results['DSP'] = int(res_node.find('DSP').text)
            results['FF'] = int(res_node.find('FF').text)
            results['LUT'] = int(res_node.find('LUT').text)
            results['SLICE'] = int(res_node.find('SLICE').text)
            results['CLB'] = int(res_node.find('CLB').text)
            results['URAM'] = int(res_node.find('URAM').text)

    return results