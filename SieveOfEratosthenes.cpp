int countPrimes(int n) {
    if(n <=1 ) return 0;
    
    vector<bool> primes(n + 1, true);
    for(int i = 2; i * i < n; i++){
        if(primes[i]){ 
            
            cout << "Counting every " << i << "th element starting from " << i * 2 << endl;
            for(int j = i * 2; j <=n; j += i){ //
                primes[j] = false;
            }
            
            cout << "All multiples of " << i << " updated." << endl;
        }
    }

    int count = 0;
    for(int i = 2; i < n; i++){
        if(primes[i]){
            count++;
        }
    }
    
    return count;
}
