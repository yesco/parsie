typedef enum { lactovegetarian, vegan, veggie, pescatore, carnevore, ominvore } foodpreference;

void print_menu(foodpreference pref, bool keto, bool monk) {
  switch(pref) {
  case omivore;
    if (!keto) printf("pasta with bechamel and mushrooms\n");
    printf("silkcocoons\n");
  case carnevore:
    printf("HK style pidgeon\n");
    printf("steak tartar\n");
    printf("chicken\n");
    printf("tomahawk\n");
  case pescatore:
    printf("sushi\n");
    if (!keto) printf("fish and chips\n");
  case veggie:
    printf("egg salad\n");
    if (!keto) printf("pasta with bechamel and mushrooms\n");
    printf("frozen yogurt, no sugar\n");
    if (!keto) printf("honey-glazed walnuts\n");
  case vegan:
    if (!keto) printf("mushrooms risotto\n");
    if (!keto) printf("stirfry carrots\n");
    if (!monk) printf("garlic fried broccoli\n");
  case lactovegetarian:
    printf("fried tofu\n");
    printf("green leaf salad\n");
    printf("pumpkin soup\n");
  }
}
