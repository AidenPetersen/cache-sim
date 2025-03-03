SCRIPTS_DIR=../../scripts
all: convert_trace

.PHONY: convert_trace
convert_trace: trace
	python3 $(SCRIPTS_DIR)/val2trace.py trace_tmp.txt trace.txt
	rm trace_tmp.txt

.PHONY: trace
trace:
	$(SHELL) -c './gen_trace.sh'
