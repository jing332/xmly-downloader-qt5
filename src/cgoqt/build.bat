set GOARCH=386
set CGO_ENABLED=1
go build -buildmode=c-archive
pause