# KTLT Telecommunications Subscriber Management Platform

Welcome to the institutional technical documentation for the **Mobile Subscriber & Telecom Management System** (Hệ thống Quản lý Thuê bao Di động Viễn thông) developed at the Posts and Telecommunications Institute of Technology (PTIT).

---

## Document Index & Core Specifications

### 1. Canonical Technical Specifications
* **[Technical Manual & System Architecture Specification](Reports/KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.md)**: Comprehensive, publication-grade technical manual covering 3GPP IMEI/EIR compliance, POSIX atomic storage engine, C++11 OOP layered design, verification pyramid, and defense runbooks.
* **[Architecture & Requirements Overview](Overview/ARCHITECTURE_AND_REQUIREMENTS.md)**: Baseline functional requirements, domain entities, and data relationships.
* **[Technical Design Specification](Operational/TECHNICAL_DESIGN_SPECIFICATION.md)**: Component breakdown, inheritance diagrams, and class responsibilities.

### 2. Operational & Protocol Specifications
* **[Function Callflow Specification](Operational/FUNCTION_CALLFLOW_SPEC.md)**: High-fidelity call flows from console menu controllers down to low-level persistence.
* **[Call Detail Records (CDR / GhiCuocGoi) Specification](Operational/CDR_GHICUOCGOI.md)**: Telecommunications CDR processing, duration rating, and invoice aggregation.
* **[Interaction & Sequence Diagrams](Operational/INTERACTION_DIAGRAM.md)**: Detailed UML interaction diagrams for registration, SIM binding, and top-up.
* **[Lead Architect Oral Defense Guide](Operational/IMEI_SHARED_ORAL_DEFENSE.md)**: Structured defense questions, algorithmic breakdowns, and oral defense guide.

### 3. User & Console Interface Guides
* **[Console User Interface & Layout Specification](Display/CONSOLE_UI_SPEC.md)**: Universal framing, box-drawing rules, and user interaction standards.
* **[End-User Operational Manual](UserGuide/USER_MANUAL.md)**: Step-by-step administrator and operator workflows for all 7 console menus.
* **[Engineering Function & Process Map](engineering/function_process_map.md)**: Complete cross-reference of functions, models, and file I/O operations.

---

## Authoritative Team Roster

| Member | Student ID (MSV) | Assigned Subsystem & Use Cases |
|---|---|---|
| **Trần Đức Anh (Lead)** | `B24DCVT021` | **UC01 (HopDong)** & **UC02 (ThietBiIMEI)**, Core Shared Libraries |
| **Trương Quốc Khánh** | `B24DCVT201` | **UC03 (KhachHang)** & **UC04 (ThueBao)**, CCCD Validation |
| **Nguyễn Tùng Dương** | `B24DCVT106` | **UC05 (GoiCuoc)** & **UC06 (GoiThueBao)**, Package Lifecycle |
| **Phạm Đức Chính** | `B24DCVT047` | **UC07 (NapTien)** & **UC08 (GhiCuocGoi / CDR)**, Billing Engine |
| **Nguyễn Minh Quang** | `B24DCVT308` | **UC09 (HoaDon)** & **UC10 (KhieuNai)**, Dispute & Invoicing |
