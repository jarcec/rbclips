require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Rule)
    assert Clips::Rule.constants.member?(:Creator)
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Rule::Creator.new }
    assert_raise(Clips::ArgumentError)  { Clips::Rule.new(3){}  }

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animal ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animal ?b))"
    end  
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) (child-of ?a ?b) => (assert (animal ?b)))"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :a
      r.rhs '(assert (animal ?a))'
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) => (assert (animal ?a)))"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :one
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?) =>)"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :all
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal $?) =>)"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', 1, "ahoj", 2.3
    end
    assert_equal rule.to_s, '(defrule animal-mammal (animal 1 "ahoj" 2.3) =>)'
  end

  def test_save_destroy!
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert rule.save
    assert rule.destroy!
    assert !rule.destroy!
  end
end
