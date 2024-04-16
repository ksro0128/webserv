<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    echo "<h2>Uploaded File Information:</h2>";
    echo "<pre>";
    print_r($_FILES['uploadedFile']);
    echo "</pre>";

    // 파일 업로드 성공 여부를 체크
    if (isset($_FILES['uploadedFile']) && $_FILES['uploadedFile']['error'] === UPLOAD_ERR_OK) {
        // 파일 정보 가져오기
        $fileTmpPath = $_FILES['uploadedFile']['tmp_name'];
        $fileName = $_FILES['uploadedFile']['name'];
        $fileSize = $_FILES['uploadedFile']['size'];
        $fileType = $_FILES['uploadedFile']['type'];
        
        echo "<p>File Name: $fileName</p>";
        echo "<p>File Type: $fileType</p>";
        echo "<p>File Size: $fileSize bytes</p>";

        // 파일을 최종적으로 저장할 위치와 파일명
        $dest_path = './' . $fileName;

        // 파일 이동 시도
        if (move_uploaded_file($fileTmpPath, $dest_path)) {
            echo "<p>File is successfully uploaded to $dest_path</p>";
        } else {
            echo "<p>Error occurred while uploading the file to $dest_path</p>";
        }
    } else {
        echo "<p>Error: " . $_FILES['uploadedFile']['error'] . "</p>";
    }
} else {
    echo "<p>No file uploaded.</p>";
}
?>