# Luxora

CLI tool for data preparation.

### Features
- [x] Loading and parsing CSV files  
- [ ] Identification and processing of missing data   
    - [x] Identify empty cells  
    - [x] Imputation of mean or median of the column  
    - [ ] Arbitrary format of missing data  
- [x] Normalization of data ranges for specific columns.  
    - Both Z-score and MinMax  
- [x] Outlier detection using IQR  
- [x] Export cleaned data to a new file
- [ ] Data info  
- [ ] Time data type  
- [ ] Arbitrary Index column  
- [ ] Interactive  

### Development
- `make build` for build
- `make run -- ...` for running with args
- `make debug-test` for compiling debuggable executable of a single test
- `make test` for general testing
- Executable along with lib will be inside `build` directory

### Usage examples
- `./build/luxora-cli --file resources/timeseries.csv --column-name Value outliers --rows`  
  <pre>Outliers:  
  Index,Value,Category,Timestamp  
  4,5000.000000,A,2024-01-01 15:00:00  
  9,3000.000000,B,2024-01-01 20:0:00  
  15,-100.000000,C,2024-01-02 02:00:00</pre>
- `./build/luxora-cli --file resources/missing.csv --column-name Volume --new-column VolumeNorm normalize`  
  <label>output.csv:</label>
  <pre>
  Open,High,Low,Close,Volume,Adj Close,VolumeNorm
  64.529999,64.800003,64.139999,64.620003,21705200.000000,64.620003,1.000000
  64.419998,64.730003,64.190002,64.620003,20235200.000000,64.620003,0.398896
  64.330002,64.389999,64.050003,64.360001,19259700.000000,64.360001,0.000000
  64.610001,64.949997,64.449997,64.489998,19384900.000000,64.489998,0.051196
  64.470001,64.690002,64.300003,,21234600.000000,64.620003,0.807565
  </pre>
