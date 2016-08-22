void merge_sort(std::vector<std::string> &merged_list){
    std::vector<std::string> a, b;

    if(merged_list.size() <= 1) return;
    
    int midpoint = merged_list.size() / 2;

    a.insert(a.begin(), merged_list.begin(), merged_list.begin() + midpoint);
    b.insert(b.begin(), merged_list.begin() + midpoint, merged_list.end());

    merged_list.clear(); //clear the original container after it's broken apart.

    merge_sort(a);
    merge_sort(b);
    size_t max_size = a.size() + b.size();

    while(merged_list.size() < max_size){
        if(a.empty()){
            merged_list.insert(merged_list.end(), b.begin(), b.end());
        }
        else if(b.empty()){
            merged_list.insert(merged_list.end(), a.begin(), a.end());
        }
        else if(a.front() < b.front()){
            merged_list.push_back(a.front());
            a.erase(a.begin());
        }
        else{
            merged_list.push_back(b.front());
            b.erase(b.begin());
        }
    }
}
