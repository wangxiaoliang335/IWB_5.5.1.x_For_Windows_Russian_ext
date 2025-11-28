#OEM列表Hash表
#说明:资源路径是相对于工程IWB.vcproj所在的目录的相对路径。
$OEM_DESCRIPTION = @{ `
"1"=@{"Description" = "上海易视"              ;  "ResourceFile" = ".\IWB.rc"; "SignCmdFileName" = "Sign_EASI.cmd"}; `
"2"=@{"Description" = "新加坡Vetical-IS 3.0"  ;  "ResourceFile" = "..\OEM\Singapore\OEM.rc"; "SignCmdFileName" = "sign_VerticalMiles.cmd"}`
};
$OemCount = $OEM_DESCRIPTION.count;
$Vendor   = 1;

#提示选择OEM
Write-Host "选择OEM厂商对应的数字"  -foregroundcolor "green" -backgroundcolor "black"
foreach($oem in $OEM_DESCRIPTION.GetEnumerator())
{
    $oemDescription = $(($oem.value)["Description"]); 
    Write-Host "[$($oem.key)]:$oemDescription" -foregroundcolor "yellow"   -backgroundcolor "black"
 }
  
 do
 {
    $Vendor = Read-Host "选择哪个OEM[1~$OemCount]" ;    
     
 }while(([int]$Vendor -gt $OemCount)  -or  ([int]$NewVendor -lt 0)); 
 
 
 #替换工程中的资源文件
 #替换VS2008工程文件IWB.vcproj中的.rc文件的路径
 $NewResourceFilePath = $($OEM_DESCRIPTION[$Vendor])["ResourceFile"];
 
 $ProjFilePath=".\IWB\IWB.vcproj";
 
 Write-Host "Resource File Path = $NewResourceFilePath."
  
 #是利用正则表达式替换工程文件中的"(\S+).rc" 
 #(\S+)表示一个或者多个分空格字符。
 (Get-Content $ProjFilePath) | ForEach-Object {$_ -replace "`"\S+\.rc`"", "`"$NewResourceFilePath`""} | Out-File "$ProjFilePath" -Encoding "ASCII";
 
 
######################################################
#文件内容修改
######################################################
$encoding    = [System.Text.Encoding]::UTF8
#正则表达式知识
#mode modifier:
#(?ims): i:case insensitive
#        m:multiline mode, make the caret(^) and dollar($) match the start and end of each line in subject string
#        s:single line mode, mame dot(.) matche any character including line break.

$ModifyContent=@(`
@{`
    "srcFile"        = ".\IWB\Gesture Recognition\GLBoard\GestureEventGenerator.cpp";`
    "searchReg"      = "(?ms)(\s+//{{OEM}}).*?(\s+//{{OEM}})";`
    "replaceContent" = @{`
    "1"="      m_hwnd = FindWindow(NULL, _T(`"tBoard`"));";`
    "2"="      m_hwnd = FindWindow(NULL, _T(`"V-Notes 3.1`"));"`
    }`
  },`
@{`
    "srcFile"       = ".\IWB\IWB.cpp";`
    "searchReg"     = "(?ms)(\s+//{{OEM}}).*?(\s+//{{OEM}})";`
    "replaceContent"= @{`
    "1"= "      const TCHAR* szConfigUSBDiskVolumeName = _T(`"ISAESY@9168`");";`
    "2"= "      const TCHAR* szConfigUSBDiskVolumeName = _T(`"V-ID2`");"`
    }`
  }`
)


foreach($entry in $ModifyContent)
{ 
    $srcFile        = $entry["srcFile"];
    $searchRegEx    = $entry["searchReg"];
    
    #Write-Host $srcFile
    #Write-Host $searchRegEx 
    
    $replaceContent = $entry["replaceContent"];
    foreach($replace in $replaceContent.GetEnumerator())
    {
        if($replace.key -eq $Vendor)
        {
            #Write-Host $replace.value

            $fileContent = [System.IO.File]::ReadAllText($srcFile, $encoding )
            
            #在双引号中的$符号,Powershell将其作为变量替换掉
            #因此PowerShell的capture part $1,$2在双引号字符串中要用`符号转义，避免被当作PowerShell变量。
            #注意:C语言中的"\r\n"在PowerShell中应写为"`r`n"
            $reaplaceRegEx = "`$1`r`n$($replace.value)`$2";
            #Write-Host $reaplaceRegEx
            $fileContent =($fileContent -replace $searchReg, $reaplaceRegEx)
            #写入输出文件
            [System.IO.File]::WriteAllText($srcFile, $fileContent, $encoding )
        }
    }
    

}

 #修改签名批处理文件
 $SignCmdFileName =  $($OEM_DESCRIPTION[$Vendor])["SignCmdFileName"]
 $CmdFileFullPath = ".\SignTools\$SignCmdFileName"
 Copy-Item  $CmdFileFullPath ".\SignTools\Sign.cmd"
 
 

 
 
 