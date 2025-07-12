function doGet(e) {
  var ss = SpreadsheetApp.openById("1FgJf-C22navZ_Abwu2aZbgMidNrGjcrEmi8A0zJ8a4w");
  var date = new Date();
  var sheetName = Utilities.formatDate(date, "GMT+7", "dd-MM-yyyy");
  
  // Tạo sheet mới theo ngày nếu chưa tồn tại
  var sheet = ss.getSheetByName(sheetName);
  if (!sheet) {
    var originSheet = ss.getSheetByName("DSHocSinh");
    sheet = originSheet.copyTo(ss).setName(sheetName);
    // Xóa các cột không cần thiết, chỉ giữ cột UID và Tên
    sheet.deleteColumns(3, sheet.getLastColumn() - 2);
    // Thêm cột Thời gian quẹt thẻ và Trạng thái
    sheet.insertColumnsAfter(2, 2);
    sheet.getRange(1, 3).setValue("Thời gian quẹt thẻ");
    sheet.getRange(1, 4).setValue("Trạng thái");
    // Tạo trigger khi sheet được tạo
    createDailyTrigger();
  }
  
  var uid = e.parameter.uid;
  if (!uid) {
    return ContentService.createTextOutput("No UID provided");
  }
  
  var lateThreshold = new Date(date);
  lateThreshold.setHours(8, 30, 0);
  
  var originSheet = ss.getSheetByName("DSHocSinh");
  var originData = originSheet.getDataRange().getValues();
  
  for (var i = 1; i < originData.length; i++) {
    if (originData[i][0] == uid) {
      var time = Utilities.formatDate(date, "GMT+7", "HH:mm:ss");
      sheet.getRange(i + 1, 3).setValue(time); // Cột C: Thời gian quẹt thẻ
      
      // Kiểm tra trạng thái trễ
      var status = (date > lateThreshold) ? "Trễ" : "Đúng giờ";
      sheet.getRange(i + 1, 4).setValue(status); // Cột D: Trạng thái
      
      return ContentService.createTextOutput(originData[i][1]); // Trả về tên học sinh
    }
  }
  return ContentService.createTextOutput("UID not found");
}

function updateStatistics() {
  var ss = SpreadsheetApp.openById("1FgJf-C22navZ_Abwu2aZbgMidNrGjcrEmi8A0zJ8a4w");
  var date = new Date();
  var sheetName = Utilities.formatDate(date, "GMT+7", "dd-MM-yyyy");
  var sheet = ss.getSheetByName(sheetName);
  
  if (!sheet) {
    return;
  }
  
  var data = sheet.getDataRange().getValues();
  var lateCount = 0;
  var absentCount = 0;
  
  for (var i = 1; i < data.length; i++) {
    var timeCell = data[i][2]; // Cột C: Thời gian quẹt thẻ
    var statusCell = data[i][3]; // Cột D: Trạng thái
    
    if (!timeCell) {
      absentCount++;
      sheet.getRange(i + 1, 4).setValue("Vắng"); // Cột D: Trạng thái
    } else if (statusCell === "Trễ") {
      lateCount++;
    }
  }
  
  var statsRow = data.length + 2;
  sheet.getRange(statsRow, 1).setValue("Thống kê ngày " + sheet.getName());
  sheet.getRange(statsRow + 1, 1).setValue("Số học sinh đi trễ:");
  sheet.getRange(statsRow + 1, 2).setValue(lateCount);
  sheet.getRange(statsRow + 2, 1).setValue("Số học sinh vắng:");
  sheet.getRange(statsRow + 2, 2).setValue(absentCount);
}

function createDailyTrigger() {
  // Xóa các trigger cũ để tránh trùng lặp
  var triggers = ScriptApp.getProjectTriggers();
  for (var i = 0; i < triggers.length; i++) {
    if (triggers[i].getHandlerFunction() === "updateStatistics") {
      ScriptApp.deleteTrigger(triggers[i]);
    }
  }
  
  ScriptApp.newTrigger("updateStatistics")
    .timeBased()
    .everyDays(1)
    .atHour(22)
    .inTimezone("Asia/Ho_Chi_Minh")
    .create();
}