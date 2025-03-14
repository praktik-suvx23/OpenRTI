bool userInteraction() {
    std::string input = "temp";
    while(true){
        std::getline(std::cin, input);
        if (input == "") {
            break;
        }
    }
    return true;
}