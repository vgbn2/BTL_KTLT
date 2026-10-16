#include<iostream>
#include<string>
#include<vector>
#include<iomanip>
using namespace std;
class goicuoc {
private:
	string magoi;
	string tengoicuoc;
	double giacuoc;
	int dungluongdata;
	int sophutgoi;
public:
	goicuoc{}
		goicuoc(tring ma, string ten, double gia, int data, int goi) {
		magoi = ma;
		tengoicuoc = ten;
		giacuoc = gia;
		dungluongdata = data;
		sophutgoi = goi;
	}
	string getMagoi() const { return magoi; }
	string gettengoi() const { return tengoi; }
	double getgiacuoc() const { return giacuoc; }
	void hienthi() const {
		cout << left << setw(10) << magoi << setw(20) << tengoi << setw(12) << giacuoc << setw(15) << (to_string(dungluongdata + " GB ") << setw(15) << (to_string(sophutgoi) + "phút") << endl;
	}
	class khachhang{
	private:
		string maKH;
		string hoTen;
		string cccd;
		string soDienThoaiLienLac;
	public:
		Khachhang() {}
		Khachhang(string ma, string ten, string id, string sdt) {
			maKH = ma;
			hoTen = ten;
			cccd = id;
			soDienThoaiLienLac = sdt;
		}
		void hienthi() const {
			cout << "Mã KH: " << maKH << "| Họ tên: " << hoTen << "| CCCD: " << cccd << "| SĐT: " << soDienThoaiLienLac << endl;
		}
		class ThueBao{
		private:
			string soDienThoai;
			string loaiThueBao;
			KhachHang chuSoHuu;
			vector<GoiCuoc> dsGoiCuoc;
		public:
			ThueBao() {}
			ThueBao(string sdt, string loai, KhachHang chu, vector<GoiCuoc> ds) {
				soDienThoai = sdt;
				loaiThueBao = loai;
				chuSoHuu = chu;
				dsGoiCuoc = ds;
			}
			string getSoDienThoai() const { return soDienThoai; }
			void dangkiGoiCuoc(const GoiCuoc& goi) {
				dsGoiCuoc.push_back(goi);
				cout << "Đăng ký gói cước thành công!" << g.gettengoi() << " cho số " << soDienThoai << endl;
				void hienthi() const{
					cout << "---------------------------------/n";
					cout << " Số điện thoại: " << soDienThoai << "(" << loaiThueBao << ")\n"
						cout << " Chủ sở hữu: "; chuSoHuu.hienthi();
					cout << " Danh sách gói cước đã đăng ký:\n";
					if (dansachgoicuocdangdung.empty()){
						cout << " Chưa đăng ký gói cước nào.\n";
					}
					else {
						cout << left << setw(10) << "Mã gói" << setw(20) << "Tên gói cước" << setw(12) << "Giá cước" << setw(15) << "Dung lượng data" << setw(15) << "Số phút gọi" << endl;
						for (const auto& goi : dsGoiCuoc) {
							goi.hienthi();
						}

			}
