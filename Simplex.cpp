/********************************************************
* 
* 此项目为单纯形法求解线性规划问题，包含两阶段法和退化情形下的摄动法。
* 测试数据文件名为 data.txt
* 第一组数据 为简单单纯形表法；
* 第二组数据 考察两阶段法；
* 第三组数据 考察摄动法；
*
********************************************************/

#include <iostream>
#include <vector>
#include <cstdio>
#include <climits>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#define size 100

int status;
vector< vector<double> > a;
vector<double> b, c, ct, sga, x;
vector<int> eql;
vector<size_t> ar_id, base_id;
size_t st_num, x_num, x_num_, min_max;
size_t add_num=0, ba_num=0, ar_num=0;
size_t step = 1, section = 1, count = 1;
size_t in, out, inputMathod;
string datapath;
ifstream ifs;
double z=0;

void inPut() {

	while(1) {
		cout << "目标类型: 1->求极大, 2->求极小: ";
		cin >> min_max;
		cout<<endl;
		if (min_max!=1 && min_max!=2)
			cout << "error!" << endl;
		else break;
	}

	while(1) {
		cout << "结构变量x个数: ";
		cin >> x_num;
		cout<<endl;
		cout << "Input c1~c" << x_num << ":" << endl;
		cout << "c" << " = ";
		for (size_t i=0; i<x_num; ++i) {
			double c_;
			cin >> c_;
			c.push_back(c_);
		}

		if (min_max==1) cout << "max  ";
		else cout << "min  ";

		if (c[0]==1) cout << "x[1]";
		else if(c[0]==-1) cout << "-x[1]";
		else if(c[0]!=0) cout << " " << c[0] << "x[1]";
		for (size_t i=1; i<x_num; ++i) {
			if(c[i]==1) cout << " + x[" << i+1 << "]";
			else if (c[i]==-1) cout << " - x[" << i+1 << "]";
			else if (c[i]>0) cout << " + " << c[i] << "x[" << i+1 << "]";
			else if (c[i]<0) cout << " - " << -c[i] << "x[" << i+1 << "]";
		}
		cout << "\t";
A1: cout << "correct?:(y/n)";
		char yn;
		cin >> yn;
		if (yn == 'y') break;
		else if (yn == 'n') continue;
		else {
			cout << "error!" << endl;
			goto A1;
		}
	}

	cout <<endl<< "约束方程s.t个数: ";
	cin  >> st_num;
	cout<<endl;
	eql.resize(size);
	b.resize(size);
	base_id.resize(size);
	ar_id.resize(size);

	for (size_t i=0; i<st_num; ++i) {
		cout << "s.t." << i+1 << ":" << endl;
		while (1) {
			cout << "约束类型: 1、==; 2、<=, 3、>= ： ";
			cin >> eql[i];
			if(eql[i]!=1 && eql[i]!=2 && eql[i]!=3)
				cout << "error!" << endl;
			else break;
		}

		cout << "左端系数 a: " << endl;
		vector<double> a_(x_num);
		cout << "a1~a" << x_num << " = ";
		for (size_t j=0; j<x_num; ++j) 
			cin >> a_[j];
		a.push_back(a_);

		cout << "右端系数 b: " << endl;
		cout << "b" << i+1 << " = ";
		cin >> b[i];
		cout << "st." << i+1 << ": ";
		if (a[i][0]==1) cout << "x[1]";
		else if(a[i][0]==-1) cout << "-x[1]";
		else if(a[i][0]!=0) cout << " " << a[i][0] << "x[1]";
		for (size_t j=1; j<x_num; ++j) {
			if(a[i][j]==1) cout << " + x[" << j+1 << "]";
			else if (a[i][j]==-1) cout << " - x[" << j+1 << "]";
			else if (a[i][j]>0) cout << " + " << a[i][j] << "x[" << j+1 << "]";
			else if (a[i][j]<0) cout << " - " << -a[i][j] << "x[" << j+1 << "]";
		}
		switch (eql[i]) {
		case 1: cout << " == " << b[i]; break;
		case 2: cout << " <= " << b[i]; break;
		case 3: cout << " >= " << b[i]; break;
		}

		while (1) {
			cout << "\t" ;
			cout << "correct?:(y/n)";
			char yn;
			cin >> yn;
			if (yn == 'y') break;
			else if(yn == 'n') {--i; a.pop_back();break;}
		}
		cout << endl;
	}

	c.resize(size);
	a.resize(size);

}

int dataInput() {
	string line;
	if(!getline(ifs, line))return -1;
	getline(ifs, line);
	stringstream sst(line);

	sst >> min_max >>  x_num >>st_num;

	c.resize(size);
	a.resize(size);
	b.resize(size);
	eql.resize(size);
	base_id.resize(size);
	ar_id.resize(size);

	sst.clear();
	getline(ifs, line);
	sst.str(line);
	for (size_t i=0; i<x_num; ++i) sst >> c[i];

	for (size_t i=0; i<st_num; ++i) {
		vector<double> a_(size);
		sst.clear();
		getline(ifs, line);
		//cout << line << endl;
		sst.str(line);
		for (size_t j=0; j<x_num; ++j) {
			sst >> a_[j];
		}
		a[i] = a_;
	}

	sst.clear();
	getline(ifs, line);
	sst.str(line);
	for (size_t i=0; i<st_num; ++i) sst >> eql[i];


	sst.clear();
	getline(ifs, line);
	sst.str(line);
	for (size_t i=0; i<st_num; ++i) sst >> b[i];

	return 0;
}

void showOr() {
	cout << endl << "初始问题: " << endl;
	if (min_max == 1) cout << "max  ";
	else cout << "min  ";
	if (c[0]==1) cout << "x[1]";
	else if(c[0]==-1) cout << "-x[1]";
	else if(c[0]!=0) cout << " " << c[0] << "x[1]";
	for (size_t i=1; i<x_num; ++i) {
		if(c[i]==1) cout << " + x[" << i+1 << "]";
		else if (c[i]==-1) cout << " - x[" << i+1 << "]";
		else if (c[i]>0) cout << " + " << c[i] << "x[" << i+1 << "]";
		else if (c[i]<0) cout << " - " << -c[i] << "x[" << i+1 << "]";
	}


	cout << endl << "st: " << endl;
	for (size_t i=0; i<st_num; ++i) {
		for (size_t j=0; j<x_num; ++j) {
			if (a[i][j] != 0) {
				if (a[i][j]==1 && j==0) cout << " x[" << j+1 << "]";
				else if (a[i][j]==1 && j!=0) cout << " + x[" << j+1 << "]";
				else if (a[i][j]==-1 && j==0) cout << " -x[" << j+1 << "]";
				else if (a[i][j]==-1 && j!=0) cout << " - x[" << j+1 << "]";
				else if (a[i][j]>0 && j==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]>0 && j!=0) cout << " + " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && j==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && j!=0) cout << " - " << -a[i][j] << "x[" << j+1 << "]";
			}
		}
		switch (eql[i]) {
		case 1:
			cout << " == " << b[i];
			break;
		case 2:
			cout << " <= " << b[i];
			break;
		case 3:
			cout << " >= " << b[i];
			break;
		}
		cout << endl;
	}
	cout << " x[1]~x[" << x_num << "] >= 0" << endl << endl;



}

void normalize() {
	x_num_ = x_num;
	for (size_t i=0; i<st_num; ++i) {
		switch (eql[i]) {
		case 1:
		case 2:
			x_num++;
			break;
		case 3:
			x_num += 2;
			break;
		}
	}
	c.resize(x_num);
	for (size_t i=0; i<st_num; ++i) {
		a[i].resize(x_num);
	}

	if (min_max == 2)
		for (size_t i=0; i<x_num_; ++i) c[i] *= -1;

	for (size_t i=0; i<st_num; ++i) {
		switch (eql[i]) {
		case 1:
			break;
		case 3:
			add_num++;
			c[x_num_+add_num-1]=0;
			for(size_t j=x_num_; j<x_num; j++)
				if(j==x_num_+add_num-1) a[i][x_num_+add_num-1]=-1;
			break;
		case 2:
			add_num++;
			ba_num++;
			c[x_num_+add_num-1]=0;
			base_id[ba_num-1] = x_num_ + add_num;
			for(size_t j=x_num_; j<x_num; j++)
				if(j==x_num_+add_num-1) a[i][j]=1;
			break;
		}
	}

	for (size_t i=0; i<st_num; ++i) {
		switch (eql[i]) {
		case 1:
			add_num++;
			ba_num++;
			ar_num++;
			c[x_num_+add_num-1] = 0;
			base_id[ba_num-1] = ar_id[ar_num-1] = x_num_ + add_num;
			for(size_t j=x_num_; j<x_num; j++)
				if(j==x_num_+add_num-1) a[i][j] = 1;
			break;
		case 3:
			add_num++;
			ba_num++;
			ar_num++;
			c[x_num_+add_num-1]=0;
			base_id[ba_num-1] = ar_id[ar_num-1] = x_num_ + add_num;
			for(size_t j=x_num_;j<x_num;j++)
				if(j==x_num_+add_num-1) a[i][j] = 1;
			break;
		case 2:
			break;
		}
	}
	vector<size_t> base_id_;
	for (size_t i=0; i<st_num; ++i) {
		for (size_t j=0; j<ba_num; ++j)
			if (a[i][base_id[j]-1] == 1)
				base_id_.push_back(base_id[j]);
	}
	base_id = base_id_;

	cout << "问题标准化: " << endl;
	if (min_max == 1) cout << "max  ";
	else cout << "min  ";
	if (c[0]==1) cout << "x[1]";
	else if(c[0]==-1) cout << "-x[1]";
	else if(c[0]!=0) cout << " " << c[0] << "x[1]";
	for (size_t i=1; i<x_num; ++i) {
		if(c[i]==1) cout << " + x[" << i+1 << "]";
		else if (c[i]==-1) cout << " - x[" << i+1 << "]";
		else if (c[i]>0) cout << " + " << c[i] << "x[" << i+1 << "]";
		else if (c[i]<0) cout << " - " << -c[i] << "x[" << i+1 << "]";
	}

	cout << endl << "s.t.: " << endl;
	for (size_t i=0; i<st_num; ++i) {
		for (size_t j=0; j<x_num; ++j) {
			if (a[i][j] != 0) {
				if (a[i][j]==1 && j==0) cout << " x[" << j+1 << "]";
				else if (a[i][j]==1 && j!=0) cout << " + x[" << j+1 << "]";
				else if (a[i][j]==-1 && j==0) cout << " -x[" << j+1 << "]";
				else if (a[i][j]==-1 && j!=0) cout << " - x[" << j+1 << "]";
				else if (a[i][j]>0 && j==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]>0 && j!=0) cout << " + " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && j==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && j!=0) cout << " - " << -a[i][j] << "x[" << j+1 << "]";
			}
		}
		cout << " = " << b[i] << endl;
	}
	cout << " x[1]~x[" << x_num << "] >= 0" << endl << endl;
}

void showPr() {
	if (section == 1) {
		cout << "min  x[" << ar_id[0] << "]";
		for (size_t i=1; i<ar_num; ++i) {
			cout << " + x[" << ar_id[i] << "]";
		}
	} else if (section == 2) {
		cout << "min  ";
		if (c[0]==1) cout << "x[1]";
		else if(c[0]==-1) cout << "-x[1]";
		else if(c[0]!=0) cout << " " << c[0] << "x[1]";
		for (size_t i=1; i<x_num; ++i) {
			if(c[i]==1) cout << " + x[" << i+1 << "]";
			else if (c[i]==-1) cout << " - x[" << i+1 << "]";
			else if (c[i]>0) cout << " + " << c[i] << "x[" << i+1 << "]";
			else if (c[i]<0) cout << " - " << -c[i] << "x[" << i+1 << "]";
		}
	} else cout << "Section wrong" << endl;


	cout << endl << "s.t.: " << endl;
	for (size_t i=0; i<st_num; ++i) {
		int k = 0;
		for (size_t j=0; j<x_num; ++j) {
			if (a[i][j] != 0) {
				if (a[i][j]==1 && k==0) cout << " x[" << j+1 << "]";
				else if (a[i][j]==1 && k!=0) cout << " + x[" << j+1 << "]";
				else if (a[i][j]==-1 && k==0) cout << " -x[" << j+1 << "]";
				else if (a[i][j]==-1 && k!=0) cout << " - x[" << j+1 << "]";
				else if (a[i][j]>0 && k==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]>0 && k!=0) cout << " + " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && k==0) cout << " " << a[i][j] << "x[" << j+1 << "]";
				else if (a[i][j]<0 && k!=0) cout << " - " << -a[i][j] << "x[" << j+1 << "]";
				k = 1;
			}
		}
		cout << " = " << b[i] << endl;
	}
	cout << " x[1]~x[" << x_num << "] >= 0" << endl << endl;;
}

void outPut() {
	cout <<"第 "<< count << " 次迭代 : " << endl;
	cout << "c" << "\t";
	for ( size_t i=0; i<x_num; ++i )
		cout << c[i] << "\t";
	cout << z << endl;

	cout << " " << "\t";
	for ( size_t i=0; i<x_num; ++i ) {
		cout << "x" << i+1 << "\t";
	}
	cout << "b" << "\t" << endl;

	for ( size_t i=0; i<st_num; ++i ) {
		cout << "x[" << base_id[i] << "]\t";
		for( size_t j = 0 ; j < x_num ; ++j )
			cout << a[i][j] << "\t";
		cout << b[i] << "\t" << endl;
	}

	cout << "判别数" << "\t";
	for (size_t i=0; i<x_num; ++i) {
		cout << sga[i] << "\t" ;
	}
	cout << endl;
}

void _result() {
	x.resize(x_num);
	cout << " *x = (";
	for (size_t i=0; i<x_num; ++i) {
		for (size_t j=0; j<st_num; ++j) {
			if (i==base_id[j]-1) {
				x[i] = b[j];
				break;
			} else x[i] = 0;
		}
		cout << x[i]<< "  ";
	}
	cout << ")\t";
	switch(min_max)
	{
		case 1:cout << " *f = " << -1*z << endl << endl;break;
		case 2:cout << " *f = " << z << endl << endl;break;
	}
}

void _iterator() {
	size_t k_f, k_a;
	size_t mainElerow;
	double mainElevalue;


	//sigma
	sga.resize(x_num);
	for (size_t i=0; i<x_num; ++i) {
		sga[i] = c[i];
		for (size_t j=0; j<st_num; ++j)
			sga[i] -= c[base_id[j]-1] * a[j][i];
		for (size_t j=0; j<st_num; ++j)
			if (i==base_id[j]-1) sga[i] = 0;
	}

	z = 0;
	for (size_t j=0; j<st_num; ++j)
		z -= c[base_id[j]-1] * b[j];

	outPut();

	size_t k_ = 0;
	for (; k_<x_num; ++k_)
		if (sga[k_] > 0) break;
	if (k_ == x_num) {
		for (size_t i=0; i<x_num; ++i) {
			k_f = k_a = 0;
			for(size_t j=0; j<ar_num; ++j)
				if(i==ar_id[j]) k_a=1;
			if(sga[i]==0 && k_a!=1) {
				for(size_t j=0; j<st_num; ++j) if(i==base_id[j]-1) k_f=1;
				if(k_f==0) {status=-1; return;}
			}
		}
		status = 1;
		return;
	}

	for(size_t i=0; i<x_num; ++i) {
		k_f = 0;
		if(sga[i] > 0) {
			for(size_t j=0; j<st_num; ++j) if(a[j][i]>0) k_f=1;
			if(k_f!=1) {status=0; return;}
		}
	}


	//base in
	double sgaMax = -INT_MAX;
	for (size_t i=0; i<x_num; ++i) {
		if (sga[i]>0 && sgaMax<sga[i] ) {
			in = i;
			sgaMax = sga[i];
		}
	}

	double staMin = INT_MAX;
	for (size_t i=0; i<st_num; ++i) {
		if (b[i]/a[i][in]<=staMin && a[i][in]>0) {
			bool exc = false;
			if(b[i]/a[i][in] == staMin) {
				for (size_t j=0; j<x_num; ++j) {
					if (j == in) continue;
					if (a[i][j]/a[i][in]<a[mainElerow][j]/a[mainElerow][in]) {
						exc = true;
						break;
					}
				}

			} else exc = true;
			if (exc) {
				out = base_id[i];
				mainElerow = i;
				staMin = b[i]/a[i][in];
			}
		}
	}


	cout << "基: (";
	for (size_t i=0; i<st_num; ++i)
		cout << " x[" << base_id[i] << "]";
	cout << ")\t\t";
	cout << "离基变量: x[" << out << "]\t\t";
	cout << "进基变量: x[" << in+1 << "]" << endl << endl;

	for (size_t i=0; i<st_num; ++i)
		if (base_id[i]==out) base_id[i]=in+1;

	mainElevalue = a[mainElerow][in];
	b[mainElerow] /= mainElevalue;
	for (size_t i=0; i<x_num; ++i) a[mainElerow][i]/=mainElevalue;
	for (size_t i=0; i<st_num; ++i) {
		if (i!=mainElerow) {
			b[i] -= b[mainElerow]*a[i][in];
			mainElevalue = a[i][in];
			for (size_t j=0; j<x_num; ++j)
				a[i][j] -= a[mainElerow][j]*mainElevalue;
		}
	}

	++count;
	if (count > 200) {
		status = -2;
		return;
	}

	_iterator();

}

void dataClear() {
	a.clear();  b.clear();  c.clear();
	ct.clear(); sga.clear();    x.clear();
	eql.clear();
	ar_id.clear();  base_id.clear();
	add_num = 0;    ba_num = 0;     ar_num = 0;
	step = 1;       section = 1;    count = 1;
	z = 0;
}

int main()
{
	while (1) {
		cout << " 1.加载数据.\t 2.输入数据 ";
		cin >> inputMathod;
		if (inputMathod==1 || inputMathod==2) break;
		else cout << "error!" << endl;
	}

	int dataCount = 1;
	if (inputMathod == 1) {
		cout<<"输入文件名(data.txt): ";
		cin>>datapath;
		ifs.open(datapath);
		if (!ifs.is_open()) {
			cout << "打开文件错误" << endl;
			exit(0);
		}
	}

	while (inputMathod) {

		if (inputMathod == 2) inPut();
		else {
			if (dataInput() == -1) {
				cout << "无数据!" << endl;

				size_t st = 0;
				while (1) {
					cout << " 1.输入数据\t 2.结束 ";
					cin >> st ;
					if (st!=1 && st!=2) {
						cout << "error!" << endl;
						continue;
					} else  break;
				}
				if (st == 1) {
					inputMathod = 2;
					continue;
				} else return 0;
			}
			cout << endl;
		}

		switch(dataCount)
		{
			case 1:cout<<"两阶段法<例3.2.2>"<<endl;break;
			case 2:cout<<"摄动法<例3.3.1>"<<endl;break;
		}
		showOr();

		normalize();

		ct.resize(x_num);

		for (size_t i=0; i<x_num; ++i) {
			size_t j = 0;
			for (; j<ar_num; ++j)
				if (ar_id[j]-1 == i) break;
			if (j == ar_num) ct[i] = 0;
			else ct[i] = -1;
		}
		swap(c, ct);
		cout << "第一阶段 : " << endl;
// 		cout << "结果 : "<<endl;
		showPr();
		_iterator();
		if (status == -2) {
			cout << "超时" << endl;
			exit(0);
		}
		_result();

		if (z != 0) {
			cout << "不存在有限最优解." << endl;
			exit(0);
		}

		swap(c, ct);
		x_num -= ar_num;
		ar_num = 0;
		cout << "第二阶段 : " << endl;
		count=1;
		showPr();
		++step;
		_iterator();
		cout << endl;

		switch (status) {
		case 1:
			cout << "最优解 : " << endl;
			_result();
			break;
		case 0:
			cout << "Unbounded solution." << endl;
			break;
		case -1:
			cout << "交替最优解 :" << endl;
			_result();
			break;
		case -2:
			cout << "迭代超时." << endl;
			break;
		}
		size_t st = 0;
		while (1) {
			cout << " 1.加载其他数据\t 2.结束 ";
			cin >> st ;
			if (st!=1 && st!=2) {
				cout << "error!" << endl;
				continue;
			} else  break;
		}
		if (st == 1) {
			dataClear();
			dataCount++;
			continue;
		}
		else return 0;

	}

	return 0;
}