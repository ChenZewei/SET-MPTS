# SchedulingAnalysis
## Environment Building
### Third party library dependency:<br/>
  GMP library:<br/>
    GMP is a free library for arbitrary precision arithmetic, operating on signed integers, rational numbers, and floating-point numbers. There is no practical limit to the precision except the ones implied by the available memory in the machine GMP runs on. GMP has a rich set of functions, and the functions have a regular interface.<br/>
    C++ surpport is needed.<br/>
    https://gmplib.org/<br/>

  MathGL library:<br/>
    a library for making high-quality scientific graphics under Linux and Windows;<br/>
    a library for the fast data plotting and data processing of large data arrays;<br/>
    a library for working in window and console modes and for easy embedding into other programs;<br/>
    a library with large and growing set of graphics.<br/>
    http://mathgl.sourceforge.net/doc_en/Main.html<br/>
    
  TinyXML-2:<br/>
    TinyXML-2 is a simple, small, efficient, C++ XML parser that can be easily integrating into other programs.<br/>
    http://www.grinninglizard.com/tinyxml2/index.html<br/>

## Compile
enter src and make.(C++ 11 is needed)<br/>

## Run
1. Adjust the parameters in the config.xml in src;
2. Execute the program;
3. Check the numeric results and graph in results folder.

## Configure
We use XML file to save the configurations.<br/>
1. Using "parameter_name" to bound parameters;<br/>
2. Using "data" to group the data for a test;<br/>
3. The parameter content could be strings, int, double.<br/>
example:<br/>

	<parameters>
		<schedulability_test>
			<data TEST_TYPE="0">WF-DM</data>
			<data TEST_TYPE="1">WF-DM</data>
			<data TEST_TYPE="2">WF-DM</data>
			<data TEST_TYPE="8">WF-DM</data>
		</schedulability_test>
		<experiment_times>100</experiment_times>
		<lambda>
			<data>10</data>
		</lambda>
		<step>
			<data>0.1</data>
		</step>
		<init_utilization_range>
			<data>
				<min>0</min>
				<max>4</max>
			</data>
		</init_utilization_range>
		......
	</parameters>



