void extract_element(vector<string>& elements, string bufline, uint start = 0, uint num = MAX_INT, const char seperator = ',')
{
	char *charbuf;
	string cut = " \t\r\n";
	cut += seperator;

	uint count = 0;

	try
	{
		if(NULL != (charbuf = strtok(bufline.data(), cut.data())))
			do
			{
				if(count >= start && count < start + num)
				{
					elements.push_back(charbuf);
				}
				count++;
			}
			while(NULL != (charbuf = strtok(NULL, cut.data())));
	}
	catch(exception &e)
	{
		cout<<"extract exception."<<endl;
	}
}
