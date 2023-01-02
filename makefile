ARGUMENTS = data/sherlock_holmes.txt 2 "i confess"

BASENAME = n_gram_model

CPP_SOURCE = ${BASENAME}.cc
CPP_TARGET = ${BASENAME}

PYTHON_SOURCE = ${BASENAME}.py

RUST_SOURCE = ${BASENAME}.rs
RUST_TARGET = target/release/${BASENAME}

JAVA_SOURCE = ${BASENAME}.java
JAVA_TARGET = ${BASENAME}.class

all: run

compile: ${CPP_TARGET} ${JAVA_TARGET} ${RUST_TARGET} python_mypy

${CPP_TARGET}: ${CPP_SOURCE}
	@echo "==== C++ compile =================================================="
	g++ -O3 ${CPP_SOURCE} -o ${CPP_TARGET}

${JAVA_TARGET}: ${JAVA_SOURCE}
	@echo "==== Java compile ================================================="
	javac -source 11 -target 11 -Xlint ${JAVA_SOURCE}

${RUST_TARGET}: ${RUST_SOURCE}
	@echo "==== Rust compile ================================================="
	cargo build --release
	
# I know, this static checking, not compiling... But the aim is to check if it is correct.
python_mypy: ${PYTHON_SOURCE}
	@echo "==== Python compile ==============================================="
	mypy --check-untyped-defs ${PYTHON_SOURCE}

	
clean:
	rm -rf  ${CPP_TARGET} ${JAVA_TARGET} ${BASENAME}*.class ${RUST_TARGET}
	
run: compile
	@echo "==== C++ run ======================================================"
	time ./${CPP_TARGET} ${ARGUMENTS}
	@echo
	@echo "==== Python run ==================================================="
	time python3 ${PYTHON_SOURCE} ${ARGUMENTS}
	@echo
	@echo "==== Java run ====================================================="
	time java $(basename ${JAVA_TARGET}) ${ARGUMENTS}
	@echo
	@echo "==== Rust run ====================================================="
	time ./${RUST_TARGET} ${ARGUMENTS}
	
	
autoformat: google-java-format-1.15.0-all-deps.jar
	# Python
	black -l 100 ${PYTHON_SOURCE}
	isort -l 100 ${PYTHON_SOURCE}
	autopep8 --select=W291,W293 --aggressive --in-place ${PYTHON_SOURCE}
	# C++
	clang-format -i -style=file ${CPP_SOURCE}
	# Rust
	rustfmt ${RUST_SOURCE}
	# Java
	java -jar google-java-format-1.15.0-all-deps.jar --replace ${JAVA_SOURCE}

google-java-format-1.15.0-all-deps.jar:
	wget https://github.com/google/google-java-format/releases/download/v1.15.0/google-java-format-1.15.0-all-deps.jar
	